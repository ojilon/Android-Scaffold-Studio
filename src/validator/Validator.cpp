#include <validator/Validator.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace validator {

namespace {

// ---------------------------------------------------------------- characters
//
// Hand-rolled character checks instead of std::regex: libstdc++'s <regex> is a
// heavy header and is slow to compile on the low-end target machine, while
// these rules are four lines of range checks each.

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_lower(char c) {
    return c >= 'a' && c <= 'z';
}

bool is_upper(char c) {
    return c >= 'A' && c <= 'Z';
}

bool is_letter(char c) {
    return is_lower(c) || is_upper(c);
}

// [a-z][a-z0-9_]*  - Android resource names and v1 screen ids.
bool is_lower_snake(std::string_view text) {
    if (text.empty() || !is_lower(text.front()))
        return false;
    return std::all_of(text.begin() + 1, text.end(), [](char c) {
        return is_lower(c) || is_digit(c) || c == '_';
    });
}

// Java identifier: [A-Za-z_$][A-Za-z0-9_$]*
bool is_java_identifier(std::string_view text) {
    if (text.empty())
        return false;
    const char first = text.front();
    if (!(is_letter(first) || first == '_' || first == '$'))
        return false;
    return std::all_of(text.begin() + 1, text.end(), [](char c) {
        return is_letter(c) || is_digit(c) || c == '_' || c == '$';
    });
}

// Keywords that cannot be used as a Java field name. Generated code exposes
// every widget id as a protected field, so `"id": "class"` would emit code
// that does not compile.
bool is_java_keyword(std::string_view text) {
    static constexpr std::array<std::string_view, 53> keywords = {
        "abstract", "assert",   "boolean",  "break",      "byte",      "case",
        "catch",    "char",     "class",    "const",      "continue",  "default",
        "do",       "double",   "else",     "enum",       "extends",   "final",
        "finally",  "float",    "for",      "goto",       "if",        "implements",
        "import",   "instanceof", "int",    "interface",  "long",      "native",
        "new",      "package",  "private",  "protected",  "public",    "return",
        "short",    "static",   "strictfp", "super",      "switch",    "synchronized",
        "this",     "throw",    "throws",   "transient",  "try",       "void",
        "volatile", "while",    "true",     "false",      "null",
    };
    return std::find(keywords.begin(), keywords.end(), text) != keywords.end();
}

// Dotted Java package name: identifier ('.' identifier)*, no keywords.
bool is_java_package(std::string_view text) {
    if (text.empty())
        return false;

    std::size_t start = 0;
    while (start <= text.size()) {
        const std::size_t dot = text.find('.', start);
        const std::size_t end = (dot == std::string_view::npos) ? text.size() : dot;
        const std::string_view segment = text.substr(start, end - start);
        if (!is_java_identifier(segment) || is_java_keyword(segment))
            return false;
        if (dot == std::string_view::npos)
            break;
        start = dot + 1;
    }
    return true;
}

// "retry" -> "onRetry": the handler name the generator will emit, so
// uniqueness is checked on what actually lands in Java.
std::string handler_name(std::string_view onClick) {
    std::string name = "on";
    if (!onClick.empty()) {
        name.push_back(static_cast<char>(onClick.front() >= 'a' && onClick.front() <= 'z'
                                             ? onClick.front() - 'a' + 'A'
                                             : onClick.front()));
        name.append(onClick.substr(1));
    }
    return name;
}

constexpr std::array<std::string_view, 7> widget_types = {
    "column", "row", "text", "button", "input", "image", "spacer",
};

constexpr std::array<std::string_view, 3> state_names = {"loading", "content", "error"};

bool is_widget_type(std::string_view type) {
    return std::find(widget_types.begin(), widget_types.end(), type) != widget_types.end();
}

bool is_state_name(std::string_view state) {
    return std::find(state_names.begin(), state_names.end(), state) != state_names.end();
}

// ------------------------------------------------------------------- errors

struct Collector {
    std::vector<RuleViolation> violations;

    void error(std::string id, std::string path, std::string message) {
        violations.push_back(RuleViolation{std::move(id), std::move(path), std::move(message), "Error"});
    }
};

// Per-screen state shared by the recursive widget walk.
struct ScreenScope {
    std::unordered_set<std::string> ids;
    std::unordered_map<std::string, std::string> handlers; // handler name -> path
    // "<screen>_state_" when the screen declares states: those ids belong to
    // the generated state views, and a model widget taking one would produce
    // two views with the same id (findViewById then returns the wrong view).
    std::string reserved_prefix;
};

void validate_widget(const parser::Widget& widget,
                     const std::string& path,
                     ScreenScope& scope,
                     Collector& out) {
    if (!is_widget_type(widget.type)) {
        out.error("V003", path + ".type", "unknown widget type \"" + widget.type + "\"");
    }

    if (!widget.id.empty()) {
        if (!is_lower_snake(widget.id)) {
            out.error("V008", path + ".id",
                      "invalid Android resource id \"" + widget.id +
                          "\" (lowercase letters, digits, underscores; must not start with a digit)");
        }
        if (is_java_keyword(widget.id)) {
            out.error("V009", path + ".id",
                      "id \"" + widget.id +
                          "\" is a Java keyword and cannot be a generated field name");
        }
        if (!scope.reserved_prefix.empty() &&
            widget.id.rfind(scope.reserved_prefix, 0) == 0) {
            out.error("V001", path + ".id",
                      "id \"" + widget.id + "\" is reserved for generated state views (" +
                          scope.reserved_prefix + "*)");
        } else if (!scope.ids.insert(widget.id).second) {
            out.error("V001", path + ".id", "duplicate id \"" + widget.id + "\"");
        }
    }

    if (!widget.onClick.empty()) {
        if (!is_java_identifier(widget.onClick)) {
            out.error("V002", path + ".onClick",
                      "invalid Java identifier \"" + widget.onClick + "\"");
        } else {
            const std::string handler = handler_name(widget.onClick);
            if (!scope.handlers.emplace(handler, path + ".onClick").second) {
                out.error("V002", path + ".onClick",
                          "duplicate onClick \"" + widget.onClick + "\" (handler " + handler +
                              " already declared at " + scope.handlers.at(handler) + ")");
            }
        }
    }

    for (std::size_t i = 0; i < widget.children.size(); ++i) {
        validate_widget(widget.children.at(i),
                        path + ".children[" + std::to_string(i) + "]",
                        scope,
                        out);
    }
}

} // namespace

core::Result<void, std::vector<RuleViolation>> validate(const parser::AppModel& model) {
    Collector out;

    // ---------------------------------------------------------------- V004/V007
    if (model.app.name.empty())
        out.error("V004", "$.app.name", "app name is empty");

    if (!is_java_package(model.app.package)) {
        out.error("V004", "$.app.package",
                  "invalid Java package name \"" + model.app.package + "\"");
    }

    static constexpr int min_supported_sdk = 21;
    static constexpr int max_supported_sdk = 35;
    if (model.app.minSdk < min_supported_sdk || model.app.minSdk > max_supported_sdk) {
        out.error("V007", "$.app.minSdk",
                  "minSdk " + std::to_string(model.app.minSdk) + " is outside the supported range " +
                      std::to_string(min_supported_sdk) + "-" + std::to_string(max_supported_sdk));
    }

    // ------------------------------------------------------------------ screens
    std::unordered_set<std::string> screen_ids;

    for (std::size_t i = 0; i < model.screens.size(); ++i) {
        const parser::Screen& screen = model.screens.at(i);
        const std::string screen_path = "$.screens[" + std::to_string(i) + "]";

        // V005: unique and usable as a file name and as the class-name stem.
        if (!is_lower_snake(screen.id)) {
            out.error("V005", screen_path + ".id",
                      "invalid screen id \"" + screen.id +
                          "\" (lowercase letters, digits, underscores; must not start with a digit)");
        } else if (!screen_ids.insert(screen.id).second) {
            out.error("V005", screen_path + ".id", "duplicate screen id \"" + screen.id + "\"");
        }

        // V006: states, when present, must be non-empty and known, and must
        // include "content": that is the state the screen's own layout lives
        // in, so without it the model's widgets would never be shown.
        if (screen.states_declared && screen.states.empty())
            out.error("V006", screen_path + ".states", "states array is empty");

        if (screen.states_declared &&
            std::find(screen.states.begin(), screen.states.end(), std::string_view("content")) ==
                screen.states.end()) {
            out.error("V006", screen_path + ".states",
                      "states must include \"content\" (the screen layout is shown there)");
        }

        for (std::size_t s = 0; s < screen.states.size(); ++s) {
            if (!is_state_name(screen.states.at(s))) {
                out.error("V006", screen_path + ".states[" + std::to_string(s) + "]",
                          "unknown state \"" + screen.states.at(s) + "\"");
            }
        }

        // V003: the layout container itself.
        if (screen.layout.type.empty()) {
            out.error("V003", screen_path + ".layout.type", "layout type is required");
        } else if (screen.layout.type != "column" && screen.layout.type != "row") {
            out.error("V003", screen_path + ".layout.type",
                      "invalid layout type \"" + screen.layout.type + "\" (use \"column\" or \"row\")");
        }

        ScreenScope scope;
        if (!screen.states.empty())
            scope.reserved_prefix = screen.id + "_state_";

        for (std::size_t c = 0; c < screen.layout.children.size(); ++c) {
            validate_widget(screen.layout.children.at(c),
                            screen_path + ".layout.children[" + std::to_string(c) + "]",
                            scope,
                            out);
        }
    }

    if (out.violations.empty())
        return {};
    return std::unexpected(std::move(out.violations));
}

} // namespace validator
