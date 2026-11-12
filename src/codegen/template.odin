package codegen

import "core:fmt"
import "core:os"
import "core:strings"

// Tiny {{dotted.path}} renderer.
// Loops / fragments are built in Odin and passed in as pre-rendered strings.

RenderError :: struct {
    message: string,
}

render :: proc(template: string, values: map[string]string) -> (string, RenderError, bool) {
    out: strings.Builder
    strings.builder_init(&out)
    defer strings.builder_destroy(&out)

    i := 0
    for i < len(template) {
        if i + 1 < len(template) && template[i] == '{' && template[i+1] == '{' {
            // find closing }}
            end := -1
            for j := i + 2; j + 1 < len(template); j += 1 {
                if template[j] == '}' && template[j+1] == '}' {
                    end = j
                    break
                }
            }
            if end < 0 {
                return "", RenderError{message = "unclosed {{ placeholder"}, false
            }
            key := strings.trim_space(template[i+2:end])
            val, found := values[key]
            if !found {
                return "", RenderError{
                    message = fmt.tprintf("unknown placeholder {{%s}}", key),
                }, false
            }
            strings.write_string(&out, val)
            i = end + 2
            continue
        }
        strings.write_byte(&out, template[i])
        i += 1
    }
    return strings.clone(strings.to_string(out)), {}, true
}

load_template :: proc(path: string) -> (string, bool) {
    data, ok := os.read_entire_file(path, context.allocator)
    if ok != nil  do return "", false
    // caller owns the returned string
    return string(data), true
}
