package parser

// App model v1 - plain value types that mirror model.json.
// Same schema as the original C++ AppModel.h.
// Unknown JSON keys are ignored so a newer model still loads.

Widget :: struct {
    type:     string,          // column, row, text, button, input, image, spacer
    id:       string,
    width:    string,          // empty => generator default
    height:   string,
    margin:   string,
    padding:  string,
    text:     string,
    on_click: string,          // handler name without the "on" prefix
    children: [dynamic]Widget,
}

Layout :: struct {
    type:     string,          // "column" or "row"
    children: [dynamic]Widget,
}

Screen :: struct {
    id:              string,
    states:          [dynamic]string, // subset of loading/content/error
    states_declared: bool,            // distinguishes missing key from empty array
    layout:          Layout,
    backend:         string,          // "" | "odin_jni" | "jni_json_bridge"
}

App :: struct {
    name:    string,
    package: string,
    min_sdk: int,
}

AppModel :: struct {
    schema_version: int,
    app:            App,
    screens:        [dynamic]Screen,
}

destroy_widget :: proc(w: ^Widget) {
    for &c in w.children {
        destroy_widget(&c)
    }
    delete(w.children)
}

destroy_layout :: proc(l: ^Layout) {
    for &c in l.children {
        destroy_widget(&c)
    }
    delete(l.children)
}

destroy_screen :: proc(s: ^Screen) {
    delete(s.states)
    destroy_layout(&s.layout)
}

destroy_model :: proc(m: ^AppModel) {
    for &s in m.screens {
        destroy_screen(&s)
    }
    delete(m.screens)
}
