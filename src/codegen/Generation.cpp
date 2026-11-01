#include <codegen/Generation.h>

#include <codegen/Template.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace codegen {

namespace {

using json = nlohmann::json;

// ------------------------------------------------------------------ strings

// XML attribute values must not carry raw markup. Android layout text is user
// data ("Fish & Chips"), so escaping is not optional.
std::string escape_xml(std::string_view text) {
    std::string escaped;
    escaped.reserve(text.size());

    for (const char c : text) {
        switch (c) {
            case '&':  escaped += "&amp;"; break;
            case '<':  escaped += "&lt;"; break;
            case '>':  escaped += "&gt;"; break;
            case '"':  escaped += "&quot;"; break;
            case '\'': escaped += "&apos;"; break;
            default:   escaped.push_back(c); break;
        }
    }
    return escaped;
}

std::string capitalize(std::string_view text) {
    std::string result(text);
    if (!result.empty() && result.front() >= 'a' && result.front() <= 'z')
        result.front() = static_cast<char>(result.front() - 'a' + 'A');
    return result;
}

// "home_state_loading" -> "homeStateLoading"
std::string camel_case(std::string_view text) {
    std::string result;
    result.reserve(text.size());

    bool upper_next = false;
    for (const char c : text) {
        if (c == '_') {
            upper_next = true;
            continue;
        }
        if (upper_next && c >= 'a' && c <= 'z') {
            result.push_back(static_cast<char>(c - 'a' + 'A'));
            upper_next = false;
        } else {
            result.push_back(c);
            upper_next = false;
        }
    }
    return result;
}

// "com.oji.hello" -> "com/oji/hello"
std::string package_directory(std::string_view package) {
    std::string path(package);
    std::replace(path.begin(), path.end(), '.', '/');
    return path;
}

// The abstract handler the generated base class declares for a widget click.
std::string handler_name(std::string_view onClick) {
    return "on" + capitalize(onClick);
}

bool has_state(const parser::Screen& screen, std::string_view state) {
    return std::find(screen.states.begin(), screen.states.end(), state) != screen.states.end();
}

// ---------------------------------------------------------------------- XML

struct XmlNode {
    std::string tag;
    std::vector<std::pair<std::string, std::string>> attributes;
    std::vector<XmlNode> children;
};

constexpr int indent_width = 4;

std::string indent(int depth) {
    return std::string(static_cast<std::size_t>(depth) * indent_width, ' ');
}

std::string render_element(const XmlNode& node, int depth) {
    const std::string pad = indent(depth);

    std::string out = pad + "<" + node.tag;
    for (const auto& [name, value] : node.attributes)
        out += "\n" + pad + indent(1) + name + "=\"" + value + "\"";

    if (node.children.empty()) {
        out += " />";
        return out;
    }

    out += ">";
    for (const XmlNode& child : node.children)
        out += "\n\n" + render_element(child, depth + 1);
    out += "\n\n" + pad + "</" + node.tag + ">";

    return out;
}

// Children block used by the templates: a leading blank line, elements
// separated by blank lines, and a blank line before the closing tag.
std::string render_children(const std::vector<XmlNode>& children) {
    if (children.empty())
        return {};

    std::string out = "\n";
    for (std::size_t i = 0; i < children.size(); ++i) {
        if (i > 0)
            out += "\n";
        out += render_element(children.at(i), 1);
        out += "\n";
    }
    return out;
}

std::string dimension(std::string_view value, std::string_view fallback) {
    return value.empty() ? std::string(fallback) : std::string(value);
}

// Model widget type -> Android view class used in the layout XML.
std::string xml_tag_for(std::string_view type) {
    if (type == "column" || type == "row") return "LinearLayout";
    if (type == "text")                    return "TextView";
    if (type == "button")                  return "Button";
    if (type == "input")                   return "EditText";
    if (type == "image")                   return "ImageView";
    if (type == "spacer")                  return "Space";
    return {}; // the validator rejects unknown types; generate() falls back below
}

// Java field type for a widget, empty when the widget gets no field.
std::string java_type_for(std::string_view type) {
    if (type == "column" || type == "row") return "LinearLayout";
    if (type == "text")                    return "TextView";
    if (type == "button")                  return "Button";
    if (type == "input")                   return "EditText";
    if (type == "image")                   return "ImageView";
    return {};
}

XmlNode widget_node(const parser::Widget& widget) {
    XmlNode node;
    node.tag = xml_tag_for(widget.type);

    const bool container = widget.type == "column" || widget.type == "row";

    if (!widget.id.empty())
        node.attributes.emplace_back("android:id", "@+id/" + escape_xml(widget.id));

    node.attributes.emplace_back("android:layout_width",
                                 escape_xml(dimension(widget.width, container ? "match_parent" : "wrap_content")));
    node.attributes.emplace_back("android:layout_height",
                                 escape_xml(dimension(widget.height, "wrap_content")));

    if (container) {
        node.attributes.emplace_back("android:orientation",
                                     widget.type == "column" ? "vertical" : "horizontal");
    }

    if (!widget.margin.empty())
        node.attributes.emplace_back("android:layout_margin", escape_xml(widget.margin));
    if (!widget.padding.empty())
        node.attributes.emplace_back("android:padding", escape_xml(widget.padding));

    if (!widget.text.empty()) {
        if (widget.type == "text" || widget.type == "button")
            node.attributes.emplace_back("android:text", escape_xml(widget.text));
        else if (widget.type == "input")
            node.attributes.emplace_back("android:hint", escape_xml(widget.text));
        else if (widget.type == "image")
            node.attributes.emplace_back("android:contentDescription", escape_xml(widget.text));
    }

    for (const parser::Widget& child : widget.children)
        node.children.push_back(widget_node(child));

    return node;
}

// The three generated state views. Wrapped in a LinearLayout each so they can
// be toggled with setVisibility() without touching the user's content layout.
XmlNode state_node(const parser::Screen& screen, std::string_view state) {
    XmlNode wrapper;
    wrapper.tag = "LinearLayout";
    wrapper.attributes.emplace_back("android:id", "@+id/" + screen.id + "_state_" + std::string(state));
    wrapper.attributes.emplace_back("android:layout_width", "match_parent");
    wrapper.attributes.emplace_back("android:layout_height", "match_parent");
    // The content wrapper mirrors the model's own container orientation.
    wrapper.attributes.emplace_back(
        "android:orientation",
        (state == "content" && screen.layout.type == "row") ? "horizontal" : "vertical");

    if (state == "loading") {
        wrapper.attributes.emplace_back("android:gravity", "center");

        XmlNode progress;
        progress.tag = "ProgressBar";
        progress.attributes.emplace_back("android:layout_width", "wrap_content");
        progress.attributes.emplace_back("android:layout_height", "wrap_content");
        wrapper.children.push_back(std::move(progress));
    } else if (state == "content") {
        for (const parser::Widget& child : screen.layout.children)
            wrapper.children.push_back(widget_node(child));
    } else { // error
        wrapper.attributes.emplace_back("android:gravity", "center");

        XmlNode message;
        message.tag = "TextView";
        message.attributes.emplace_back("android:id", "@+id/" + screen.id + "_state_error_message");
        message.attributes.emplace_back("android:layout_width", "wrap_content");
        message.attributes.emplace_back("android:layout_height", "wrap_content");
        message.attributes.emplace_back("android:text", "Something went wrong");
        wrapper.children.push_back(std::move(message));

        XmlNode retry;
        retry.tag = "Button";
        retry.attributes.emplace_back("android:id", "@+id/" + screen.id + "_state_error_retry");
        retry.attributes.emplace_back("android:layout_width", "wrap_content");
        retry.attributes.emplace_back("android:layout_height", "wrap_content");
        retry.attributes.emplace_back("android:text", "Retry");
        wrapper.children.push_back(std::move(retry));
    }

    return wrapper;
}

// ------------------------------------------------------------------ per-screen

// Every widget that gets a Java field: id present and a type we expose.
struct FieldInfo {
    std::string id;
    std::string java_type;
    std::string onClick;
};

void collect_fields(const parser::Widget& widget, std::vector<FieldInfo>& out) {
    if (!widget.id.empty()) {
        const std::string type = java_type_for(widget.type);
        if (!type.empty())
            out.push_back(FieldInfo{widget.id, type, widget.onClick});
    }
    for (const parser::Widget& child : widget.children)
        collect_fields(child, out);
}

struct ScreenCode {
    std::string imports;
    std::string fields;
    std::string find_views;
    std::string listeners;
    std::string initial_state;
    std::string handlers;
    std::string set_state;
    std::string activity_handlers;
    bool has_handlers = false;
};

ScreenCode build_screen_code(const parser::Screen& screen) {
    ScreenCode code;

    std::vector<FieldInfo> fields;
    for (const parser::Widget& child : screen.layout.children)
        collect_fields(child, fields);

    const bool states = !screen.states.empty();
    const bool error_state = has_state(screen, "error");

    // Handler names, unique, in document order; the error state's retry button
    // shares the "retry" handler so it never drifts from the user's own.
    std::vector<std::string> handlers;
    for (const FieldInfo& field : fields) {
        if (!field.onClick.empty()) {
            const std::string name = handler_name(field.onClick);
            if (std::find(handlers.begin(), handlers.end(), name) == handlers.end())
                handlers.push_back(name);
        }
    }
    if (error_state && std::find(handlers.begin(), handlers.end(), "onRetry") == handlers.end())
        handlers.push_back("onRetry");

    code.has_handlers = !handlers.empty();

    // Imports: only what the generated file actually uses, sorted like
    // Android Studio would.
    std::set<std::string> imports = {"android.os.Bundle", "androidx.appcompat.app.AppCompatActivity"};
    if (states)
        imports.insert("android.view.View");

    for (const FieldInfo& field : fields) {
        if (field.java_type == "LinearLayout") imports.insert("android.widget.LinearLayout");
        if (field.java_type == "TextView")     imports.insert("android.widget.TextView");
        if (field.java_type == "Button")       imports.insert("android.widget.Button");
        if (field.java_type == "EditText")     imports.insert("android.widget.EditText");
        if (field.java_type == "ImageView")    imports.insert("android.widget.ImageView");
    }

    std::string import_block;
    for (const std::string& name : imports)
        import_block += "import " + name + ";\n";
    if (!import_block.empty())
        import_block.pop_back(); // no trailing newline: the template owns blank lines
    code.imports = import_block;

    // Fields.
    for (const FieldInfo& field : fields)
        code.fields += "    protected " + field.java_type + " " + field.id + ";\n";

    for (const std::string& state : screen.states)
        code.fields += "    private View " + camel_case(screen.id + "_state_" + state) + ";\n";
    if (!code.fields.empty())
        code.fields.pop_back();

    // findViewById() once per id.
    code.find_views = "\n";
    for (const FieldInfo& field : fields)
        code.find_views += "        " + field.id + " = findViewById(R.id." + field.id + ");\n";
    for (const std::string& state : screen.states)
        code.find_views += "        " + camel_case(screen.id + "_state_" + state) +
                           " = findViewById(R.id." + screen.id + "_state_" + state + ");\n";
    code.find_views.pop_back();

    // Click listeners.
    for (const FieldInfo& field : fields) {
        if (!field.onClick.empty())
            code.listeners += "\n        " + field.id + ".setOnClickListener(v -> " +
                              handler_name(field.onClick) + "());";
    }
    if (error_state) {
        code.listeners += "\n        findViewById(R.id." + screen.id +
                          "_state_error_retry).setOnClickListener(v -> onRetry());";
    }

    // Initial state: the first declared one, so generated screens never open
    // in a state the developer did not ask for.
    if (!screen.states.empty())
        code.initial_state = "\n        setState(\"" + screen.states.front() + "\");\n";

    // Abstract handlers, overridden in the user-owned activity.
    for (const std::string& handler : handlers)
        code.handlers += "\n    protected abstract void " + handler + "();";

    for (const std::string& handler : handlers) {
        code.activity_handlers += "    @Override\n    protected void " + handler + "() {\n";
        code.activity_handlers += "        // TODO: implement " + handler + "()\n    }\n\n";
    }
    if (!code.activity_handlers.empty())
        code.activity_handlers.erase(code.activity_handlers.size() - 2);

    // setState(): exactly one state view visible at a time.
    if (!screen.states.empty()) {
        code.set_state = "\n    public void setState(String state) {\n";
        for (const std::string& state : screen.states) {
            code.set_state += "        " + camel_case(screen.id + "_state_" + state) +
                              ".setVisibility(\"" + state +
                              "\".equals(state) ? View.VISIBLE : View.GONE);\n";
        }
        code.set_state += "    }\n";
    }

    return code;
}

std::string manifest_activity(const parser::Screen& screen, bool launcher) {
    const std::string activity_class = capitalize(screen.id) + "Activity";

    std::string out;
    out += "        <activity\n";
    out += "            android:name=\"." + activity_class + "\"\n";
    out += std::string("            android:exported=\"") + (launcher ? "true" : "false") + "\"";

    if (!launcher) {
        out += " />\n";
        return out;
    }

    out += ">\n";
    out += "            <intent-filter>\n";
    out += "                <action android:name=\"android.intent.action.MAIN\" />\n";
    out += "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    out += "            </intent-filter>\n";
    out += "        </activity>";
    return out;
}

} // namespace

core::Result<GenerationOutput> generate(const parser::AppModel& model,
                                       const TemplateSet& templates) {
    if (model.screens.empty())
        return std::unexpected("model has no screens; nothing to generate");

    GenerationOutput output;

    for (std::size_t index = 0; index < model.screens.size(); ++index) {
        const parser::Screen& screen = model.screens.at(index);
        const ScreenCode code = build_screen_code(screen);

        const std::string screen_class = capitalize(screen.id) + "ScreenBase";
        const std::string activity_class = capitalize(screen.id) + "Activity";
        const std::string java_dir = "java/" + package_directory(model.app.package) + "/";

        // ----------------------------------------------------- layout XML
        std::vector<XmlNode> children;
        if (!screen.states.empty()) {
            for (const std::string& state : screen.states)
                children.push_back(state_node(screen, state));
        } else {
            for (const parser::Widget& child : screen.layout.children)
                children.push_back(widget_node(child));
        }

        const std::string layout_fragment = render_children(children);

        json layout_values = json::object();
        layout_values["orientation"] = screen.layout.type == "row" ? "horizontal" : "vertical";
        layout_values["children"] = layout_fragment;
        layout_values["stateViews"] = layout_fragment;

        const std::string& layout_template =
            screen.states.empty() ? templates.layout : templates.layout_states;
        auto layout_xml = render(layout_template, json{{"layout", layout_values}},
                                 "layout.xml.in");
        if (!layout_xml)
            return std::unexpected(layout_xml.error());

        output.files.push_back(GeneratedFile{
            core::fs::Path("res/layout/" + screen.id + ".xml"), std::move(*layout_xml), false});

        // -------------------------------------------------- ScreenBase.java
        json screen_values = json::object();
        screen_values["className"] = screen_class;
        screen_values["activityClass"] = activity_class;
        screen_values["baseClass"] = screen_class;
        screen_values["layoutName"] = screen.id;
        screen_values["modifiers"] = code.has_handlers ? "public abstract" : "public";
        screen_values["fields"] = code.fields;
        screen_values["findViews"] = code.find_views;
        screen_values["listeners"] = code.listeners;
        screen_values["initialState"] = code.initial_state;
        screen_values["handlers"] = code.handlers;
        screen_values["setState"] = code.set_state;

        json base_imports = json::object();
        base_imports["imports"] = code.imports;

        json base_values = json::object();
        base_values["app"] = json{{"package", model.app.package}, {"name", model.app.name}};
        base_values["screen"] = screen_values;
        base_values["java"] = base_imports;

        auto screen_base = render(templates.screen_base, base_values, "ScreenBase.java.in");
        if (!screen_base)
            return std::unexpected(screen_base.error());

        output.files.push_back(
            GeneratedFile{core::fs::Path(java_dir + screen_class + ".java"), std::move(*screen_base), false});

        // ---------------------------------------------------- Activity.java
        json activity_values = json::object();
        activity_values["app"] = json{{"package", model.app.package}};
        activity_values["screen"] = json{{"activityClass", activity_class},
                                        {"baseClass", screen_class},
                                        {"handlers", code.activity_handlers}};

        auto activity = render(templates.activity, activity_values, "Activity.java.in");
        if (!activity)
            return std::unexpected(activity.error());

        output.files.push_back(GeneratedFile{
            core::fs::Path(java_dir + activity_class + ".java"), std::move(*activity), true});
    }

    // ------------------------------------------------- AndroidManifest.xml
    std::string activities;
    for (std::size_t index = 0; index < model.screens.size(); ++index) {
        if (index > 0)
            activities += "\n\n";
        activities += manifest_activity(model.screens.at(index), index == 0);
    }

    json manifest_values = json::object();
    manifest_values["app"] = json{{"package", model.app.package}, {"name", model.app.name}};
    manifest_values["manifest"] = json{{"activities", activities}};

    auto manifest = render(templates.manifest, manifest_values, "AndroidManifest.xml.in");
    if (!manifest)
        return std::unexpected(manifest.error());

    output.files.push_back(
        GeneratedFile{core::fs::Path("AndroidManifest.xml"), std::move(*manifest), false});

    return output;
}

} // namespace codegen
