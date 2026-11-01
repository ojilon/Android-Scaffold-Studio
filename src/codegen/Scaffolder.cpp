#include <codegen/Scaffolder.h>

#include <cctype>
#include <sstream>
#include <string>

namespace codegen {

namespace {

std::string capitalize_first(std::string s) {
    if (!s.empty())
        s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    return s;
}

} // anonymous

core::Result<GenerationResult, std::string>
generate(const parser::AppModel& model) {

    if (model.screens.empty())
        return std::unexpected("no screens in model");

    const auto& screen = model.screens[0];   // v1: only first screen

    GenerationResult result;

    // ---------- 1. layout XML ----------
    std::ostringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml << "<!-- asstudio:generated v1 DO NOT EDIT -->\n";
    xml << "<LinearLayout xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    xml << "    android:layout_width=\"match_parent\"\n";
    xml << "    android:layout_height=\"match_parent\"\n";
    xml << "    android:orientation=\""
        << (screen.layout.type == "column" ? "vertical" : "horizontal")
        << "\">\n";

    for (const auto& child : screen.layout.children) {
        if (child.type == "text") {
            xml << "    <TextView\n"
                << "        android:id=\"@+id/" << child.id << "\"\n"
                << "        android:layout_width=\"wrap_content\"\n"
                << "        android:layout_height=\"wrap_content\"\n"
                << "        android:text=\"" << child.text << "\" />\n";
        } else if (child.type == "button") {
            xml << "    <Button\n"
                << "        android:id=\"@+id/" << child.id << "\"\n"
                << "        android:layout_width=\"wrap_content\"\n"
                << "        android:layout_height=\"wrap_content\"\n"
                << "        android:text=\"" << child.text << "\" />\n";
        }
        // other types omitted for v1
    }
    xml << "</LinearLayout>\n";
    result.layout_xml = xml.str();

    // ---------- 2. ScreenBase.java ----------
    std::string base_name = capitalize_first(screen.id) + "ScreenBase";
    std::string act_name  = capitalize_first(screen.id) + "Activity";

    std::ostringstream base;
    base << "// asstudio:generated v1 DO NOT EDIT\n"
         << "package " << model.app.package << ";\n\n"
         << "import android.os.Bundle;\n"
         << "import android.view.View;\n"
         << "import android.widget.Button;\n"
         << "import android.widget.TextView;\n"
         << "import androidx.appcompat.app.AppCompatActivity;\n\n"
         << "public class " << base_name << " extends AppCompatActivity {\n";

    for (const auto& child : screen.layout.children) {
        if (child.type == "text")
            base << "    protected TextView " << child.id << ";\n";
        else if (child.type == "button")
            base << "    protected Button " << child.id << ";\n";
    }

    base << "\n    @Override\n"
         << "    protected void onCreate(Bundle savedInstanceState) {\n"
         << "        super.onCreate(savedInstanceState);\n"
         << "        setContentView(R.layout.res_layout_" << screen.id << ");\n\n";

    for (const auto& child : screen.layout.children) {
        base << "        " << child.id << " = findViewById(R.id." << child.id << ");\n";
        if (child.type == "button" && !child.onClick.empty()) {
            base << "        " << child.id
                 << ".setOnClickListener(v -> on" << child.onClick << "());\n";
        }
    }
    base << "    }\n\n";

    for (const auto& child : screen.layout.children) {
        if (child.type == "button" && !child.onClick.empty()) {
            base << "    protected void on" << child.onClick << "() {\n"
                 << "        // abstract handler – override in Activity\n"
                 << "    }\n";
        }
    }

    base << "\n    public void setState(String state) {\n"
         << "        // loading / content / error\n"
         << "    }\n"
         << "}\n";
    result.screen_base_java = base.str();

    // ---------- 3. Activity.java (user-owned) ----------
    std::ostringstream act;
    act << "package " << model.app.package << ";\n\n"
        << "public class " << act_name << " extends " << base_name << " {\n";

    for (const auto& child : screen.layout.children) {
        if (child.type == "button" && !child.onClick.empty()) {
            act << "    @Override\n"
                << "    protected void on" << child.onClick << "() {\n"
                << "        // TODO: user implementation\n"
                << "    }\n";
        }
    }
    act << "}\n";
    result.activity_java = act.str();

    // ---------- 4. AndroidManifest.xml ----------
    std::ostringstream man;
    man << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << "<!-- asstudio:generated v1 DO NOT EDIT -->\n"
        << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n"
        << "    package=\"" << model.app.package << "\">\n\n"
        << "    <application>\n"
        << "        <activity android:name=\"." << act_name << "\">\n"
        << "            <intent-filter>\n"
        << "                <action android:name=\"android.intent.action.MAIN\" />\n"
        << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n"
        << "            </intent-filter>\n"
        << "        </activity>\n"
        << "    </application>\n"
        << "</manifest>\n";
    result.manifest_xml = man.str();

    return result;
}

} // namespace codegen