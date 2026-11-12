package validator

import "src:parser"
import "core:fmt"
import "core:strings"
import "core:unicode"

RuleViolation :: struct {
    id:      string,   // "V001", "V002", ...
    path:    string,   // JSON path
    message: string,
}

// Allowed widget types in v1
ALLOWED_TYPES := []string{"column", "row", "text", "button", "input", "image", "spacer"}

// Known screen states
KNOWN_STATES := []string{"loading", "content", "error"}

// Java keywords that must not be used as ids (become Java fields)
JAVA_KEYWORDS := []string{
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while",
}

validate :: proc(model: parser.AppModel) -> [dynamic]RuleViolation {
    violations: [dynamic]RuleViolation

    // V004 - package + app name
    if model.app.name == "" {
        append(&violations, RuleViolation{
            id = "V004", path = "$.app.name", message = "app name must be non-empty",
        })
    }
    if !is_valid_package(model.app.package) {
        append(&violations, RuleViolation{
            id = "V004", path = "$.app.package",
            message = fmt.tprintf("invalid Java package name \"%s\"", model.app.package),
        })
    }

    // V007 - minSdk range
    if model.app.min_sdk < 21 || model.app.min_sdk > 35 {
        append(&violations, RuleViolation{
            id = "V007", path = "$.app.minSdk",
            message = fmt.tprintf("minSdk %d out of supported range 21..35", model.app.min_sdk),
        })
    }

    // Screen-level rules
    seen_screen_ids: map[string]bool
    defer delete(seen_screen_ids)

    for i := 0; i < len(model.screens); i += 1 {
        s := model.screens[i]
        path_prefix := fmt.tprintf("$.screens[%d]", i)

        // V005 - screen id unique + valid
        if s.id == "" {
            append(&violations, RuleViolation{
                id = "V005", path = path_prefix + ".id", message = "screen id must be non-empty",
            })
        } else {
            if !is_valid_resource_id(s.id) {
                append(&violations, RuleViolation{
                    id = "V005", path = path_prefix + ".id",
                    message = fmt.tprintf("screen id \"%s\" is not a valid resource / class name", s.id),
                })
            }
            if seen_screen_ids[s.id] {
                append(&violations, RuleViolation{
                    id = "V005", path = path_prefix + ".id",
                    message = fmt.tprintf("duplicate screen id \"%s\"", s.id),
                })
            }
            seen_screen_ids[s.id] = true
        }

        // V006 - states
        if s.states_declared {
            if len(s.states) == 0 {
                append(&violations, RuleViolation{
                    id = "V006", path = path_prefix + ".states",
                    message = "states declared but empty",
                })
            }
            has_content := false
            for st in s.states {
                if st == "content" do has_content = true
                if !is_known_state(st) {
                    append(&violations, RuleViolation{
                        id = "V006", path = path_prefix + ".states",
                        message = fmt.tprintf("unknown state \"%s\"", st),
                    })
                }
            }
            if !has_content && len(s.states) > 0 {
                append(&violations, RuleViolation{
                    id = "V006", path = path_prefix + ".states",
                    message = "states must include \"content\"",
                })
            }
        }

        // Walk widgets for V001, V002, V003, V008, V009
        walk_widgets(&violations, s.layout.children[:], path_prefix + ".layout", s.id)
    }

    return violations
}

@(private)
walk_widgets :: proc(violations: ^[dynamic]RuleViolation, widgets: []parser.Widget, path: string, screen_id: string) {
    seen_ids: map[string]bool
    defer delete(seen_ids)
    seen_clicks: map[string]bool
    defer delete(seen_clicks)

    for i := 0; i < len(widgets); i += 1 {
        w := widgets[i]
        wpath := fmt.tprintf("%s.children[%d]", path, i)

        // V003 - type
        if !is_allowed_type(w.type) {
            append(violations, RuleViolation{
                id = "V003", path = wpath + ".type",
                message = fmt.tprintf("unknown widget type \"%s\"", w.type),
            })
        }

        // V001 / V008 / V009 - id
        if w.id != "" {
            if !is_valid_resource_id(w.id) {
                append(violations, RuleViolation{
                    id = "V008", path = wpath + ".id",
                    message = fmt.tprintf("id \"%s\" is not a valid Android resource name", w.id),
                })
            }
            if is_java_keyword(w.id) {
                append(violations, RuleViolation{
                    id = "V009", path = wpath + ".id",
                    message = fmt.tprintf("id \"%s\" is a Java keyword and cannot become a field", w.id),
                })
            }
            // Reserve state ids
            if strings.has_prefix(w.id, screen_id + "_state_") {
                append(violations, RuleViolation{
                    id = "V001", path = wpath + ".id",
                    message = fmt.tprintf("id \"%s\" is reserved for generated state views", w.id),
                })
            }
            if seen_ids[w.id] {
                append(violations, RuleViolation{
                    id = "V001", path = wpath + ".id",
                    message = fmt.tprintf("duplicate id \"%s\"", w.id),
                })
            }
            seen_ids[w.id] = true
        }

        // V002 - onClick
        if w.on_click != "" {
            if !is_valid_java_identifier(w.on_click) {
                append(violations, RuleViolation{
                    id = "V002", path = wpath + ".onClick",
                    message = fmt.tprintf("onClick \"%s\" is not a valid Java identifier", w.on_click),
                })
            }
            if seen_clicks[w.on_click] {
                append(violations, RuleViolation{
                    id = "V002", path = wpath + ".onClick",
                    message = fmt.tprintf("duplicate onClick handler \"%s\"", w.on_click),
                })
            }
            seen_clicks[w.on_click] = true
        }

        // Recurse
        if len(w.children) > 0 {
            walk_widgets(violations, w.children[:], wpath, screen_id)
        }
    }
}

@(private)
is_allowed_type :: proc(t: string) -> bool {
    for a in ALLOWED_TYPES do if a == t do return true
    return false
}

@(private)
is_known_state :: proc(s: string) -> bool {
    for k in KNOWN_STATES do if k == s do return true
    return false
}

@(private)
is_java_keyword :: proc(s: string) -> bool {
    for k in JAVA_KEYWORDS do if k == s do return true
    return false
}

@(private)
is_valid_resource_id :: proc(s: string) -> bool {
    if len(s) == 0 do return false
    if unicode.is_digit(rune(s[0])) do return false
    for r in s {
        if !(unicode.is_letter(r) || unicode.is_digit(r) || r == '_') do return false
    }
    return true
}

@(private)
is_valid_java_identifier :: proc(s: string) -> bool {
    if len(s) == 0 do return false
    if unicode.is_digit(rune(s[0])) do return false
    for r in s {
        if !(unicode.is_letter(r) || unicode.is_digit(r) || r == '_') do return false
    }
    return !is_java_keyword(s)
}

@(private)
is_valid_package :: proc(s: string) -> bool {
    if len(s) == 0 do return false
    parts := strings.split(s, ".")
    defer delete(parts)
    if len(parts) < 2 do return false
    for p in parts {
        if !is_valid_java_identifier(p) do return false
    }
    return true
}
