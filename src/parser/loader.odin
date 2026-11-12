package parser

import "core:encoding/json"
import "core:fmt"
import "core:os"
import "core:strings"

LoadError :: struct {
    path:    string,   // JSON path, e.g. "$.screens[0].id"
    message: string,
}

load_from_file :: proc(path: string) -> (AppModel, LoadError, bool) {
    data, ok := os.read_entire_file(path)
    if !ok {
        return {}, LoadError{path = path, message = "could not read file"}, false
    }
    defer delete(data)
    return load_from_bytes(data)
}

load_from_bytes :: proc(data: []byte) -> (AppModel, LoadError, bool) {
    val, err := json.parse(data, .JSON5, true)
    if err != nil {
        return {}, LoadError{path = "$", message = fmt.tprintf("JSON parse error: %v", err)}, false
    }
    defer json.destroy_value(val)

    obj, is_obj := val.(json.Object)
    if !is_obj {
        return {}, LoadError{path = "$", message = "root must be an object"}, false
    }

    model: AppModel
    model.schema_version = 1

    if sv, ok := obj["schemaVersion"]; ok {
        if n, ok := sv.(json.Integer); ok {
            model.schema_version = int(n)
        }
    }

    if app_v, ok := obj["app"]; ok {
        if app_obj, ok := app_v.(json.Object); ok {
            model.app = load_app(app_obj)
        }
    }

    if screens_v, ok := obj["screens"]; ok {
        if arr, ok := screens_v.(json.Array); ok {
            for i := 0; i < len(arr); i += 1 {
                if s_obj, ok := arr[i].(json.Object); ok {
                    screen, err := load_screen(s_obj, i)
                    if err != nil {
                        return {}, err^, false
                    }
                    append(&model.screens, screen)
                }
            }
        }
    }

    return model, {}, true
}

@(private)
load_app :: proc(obj: json.Object) -> App {
    a: App
    a.min_sdk = 21
    if v, ok := obj["name"]; ok {
        if s, ok := v.(json.String); ok do a.name = strings.clone(s)
    }
    if v, ok := obj["package"]; ok {
        if s, ok := v.(json.String); ok do a.package = strings.clone(s)
    }
    if v, ok := obj["minSdk"]; ok {
        if n, ok := v.(json.Integer); ok do a.min_sdk = int(n)
    }
    return a
}

@(private)
load_screen :: proc(obj: json.Object, index: int) -> (Screen, ^LoadError) {
    s: Screen
    if v, ok := obj["id"]; ok {
        if str, ok := v.(json.String); ok do s.id = strings.clone(str)
    }
    if v, ok := obj["backend"]; ok {
        if str, ok := v.(json.String); ok do s.backend = strings.clone(str)
    }

    if states_v, ok := obj["states"]; ok {
        s.states_declared = true
        if arr, ok := states_v.(json.Array); ok {
            for item in arr {
                if str, ok := item.(json.String); ok {
                    append(&s.states, strings.clone(str))
                }
            }
        }
    }

    if layout_v, ok := obj["layout"]; ok {
        if l_obj, ok := layout_v.(json.Object); ok {
            s.layout = load_layout(l_obj)
        }
    }

    return s, nil
}

@(private)
load_layout :: proc(obj: json.Object) -> Layout {
    l: Layout
    if v, ok := obj["type"]; ok {
        if s, ok := v.(json.String); ok do l.type = strings.clone(s)
    }
    if children_v, ok := obj["children"]; ok {
        if arr, ok := children_v.(json.Array); ok {
            for item in arr {
                if c_obj, ok := item.(json.Object); ok {
                    append(&l.children, load_widget(c_obj))
                }
            }
        }
    }
    return l
}

@(private)
load_widget :: proc(obj: json.Object) -> Widget {
    w: Widget
    if v, ok := obj["type"]; ok {
        if s, ok := v.(json.String); ok do w.type = strings.clone(s)
    }
    if v, ok := obj["id"]; ok {
        if s, ok := v.(json.String); ok do w.id = strings.clone(s)
    }
    if v, ok := obj["width"]; ok {
        if s, ok := v.(json.String); ok do w.width = strings.clone(s)
    }
    if v, ok := obj["height"]; ok {
        if s, ok := v.(json.String); ok do w.height = strings.clone(s)
    }
    if v, ok := obj["margin"]; ok {
        if s, ok := v.(json.String); ok do w.margin = strings.clone(s)
    }
    if v, ok := obj["padding"]; ok {
        if s, ok := v.(json.String); ok do w.padding = strings.clone(s)
    }
    if v, ok := obj["text"]; ok {
        if s, ok := v.(json.String); ok do w.text = strings.clone(s)
    }
    if v, ok := obj["onClick"]; ok {
        if s, ok := v.(json.String); ok do w.on_click = strings.clone(s)
    }
    if children_v, ok := obj["children"]; ok {
        if arr, ok := children_v.(json.Array); ok {
            for item in arr {
                if c_obj, ok := item.(json.Object); ok {
                    append(&w.children, load_widget(c_obj))
                }
            }
        }
    }
    return w
}
