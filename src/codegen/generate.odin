package codegen

import "src:parser"
import "core:fmt"
import "core:strings"
import "core:unicode"

GeneratedFile :: struct {
    path:       string,
    content:    string,
    user_owned: bool,
}

generate_all :: proc(model: parser.AppModel, templates_dir: string) -> ([dynamic]GeneratedFile, string) {
    files: [dynamic]GeneratedFile
    _ = templates_dir

    manifest := generate_manifest(model)
    append(&files, GeneratedFile{path = "AndroidManifest.xml", content = manifest})

    for i := 0; i < len(model.screens); i += 1 {
        s := model.screens[i]
        class_name := screen_class_name(s.id)
        pkg_path := package_to_path(model.app.pckg)

        layout_xml := generate_layout_xml(s)
        append(&files, GeneratedFile{
            path    = fmt.tprintf("res/layout/%s.xml", s.id),
            content = layout_xml,
        })

        base_java := generate_screen_base(model, s, class_name)
        append(&files, GeneratedFile{
            path    = fmt.tprintf("java/%s/%sScreenBase.java", pkg_path, class_name),
            content = base_java,
        })

        act_java := generate_activity(model, s, class_name)
        append(&files, GeneratedFile{
            path       = fmt.tprintf("java/%s/%sActivity.java", pkg_path, class_name),
            content    = act_java,
            user_owned = true,
        })
    }

    // Gradle shell (subfolder; does not disturb Phase-1 golden paths)
    append_gradle_project(&files, model)

    // Odin engine + Java NativeBridge when a screen asks for a native backend
    append_native_engine(&files, model)

    return files, ""
}

@(private)
generate_manifest :: proc(model: parser.AppModel) -> string {
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    strings.write_string(&b, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
    strings.write_string(&b, "<!-- asstudio:generated v1 DO NOT EDIT -->\n")
    fmt.sbprint(&b, "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n    package=\"%s\">\n\n", model.app.pckg)
    strings.write_string(&b, "    <application>\n")
    for i := 0; i < len(model.screens); i += 1 {
        s := model.screens[i]
        class_name := screen_class_name(s.id)
        exported := "false"
        if i == 0 do exported = "true"
        strings.write_string(&b, fmt.tprintf("        <activity\n            android:name=\".%sActivity\"\n            android:exported=\"%s\">\n", class_name, exported))
        if i == 0 {
            strings.write_string(&b, "            <intent-filter>\n")
            strings.write_string(&b, "                <action android:name=\"android.intent.action.MAIN\" />\n")
            strings.write_string(&b, "                <category android:name=\"android.intent.category.LAUNCHER\" />\n")
            strings.write_string(&b, "            </intent-filter>\n")
        }
        strings.write_string(&b, "        </activity>\n")
    }
    strings.write_string(&b, "    </application>\n\n</manifest>\n")
    return strings.clone(strings.to_string(b))
}

@(private)
generate_layout_xml :: proc(s: parser.Screen) -> string {
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    strings.write_string(&b, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
    strings.write_string(&b, "<!-- asstudio:generated v1 DO NOT EDIT -->\n")
    if len(s.states) > 0 {
        strings.write_string(&b, "<FrameLayout xmlns:android=\"http://schemas.android.com/apk/res/android\"\n")
        strings.write_string(&b, "    android:layout_width=\"match_parent\"\n")
        strings.write_string(&b, "    android:layout_height=\"match_parent\">\n\n")
        for st in s.states {
            write_state_view(&b, s, st)
        }
        strings.write_string(&b, "</FrameLayout>\n")
    }
    return strings.clone(strings.to_string(b))
}

@(private)
write_state_view :: proc(b: ^strings.Builder, s: parser.Screen, state: string) {
    id := fmt.tprintf("%s_state_%s", s.id, state)
    strings.write_string(b, fmt.tprintf("    <LinearLayout\n        android:id=\"@+id/%s\"\n", id))
    strings.write_string(b, "        android:layout_width=\"match_parent\"\n")
    strings.write_string(b, "        android:layout_height=\"match_parent\"\n")
    strings.write_string(b, "        android:orientation=\"vertical\"")
    if state == "loading" || state == "error" {
        strings.write_string(b, "\n        android:gravity=\"center\"")
    }
    strings.write_string(b, ">\n\n")
    switch state {
    case "loading":
        strings.write_string(b, "        <ProgressBar\n")
        strings.write_string(b, "            android:layout_width=\"wrap_content\"\n")
        strings.write_string(b, "            android:layout_height=\"wrap_content\" />\n\n")
    case "error":
        strings.write_string(b, fmt.tprintf("        <TextView\n            android:id=\"@+id/%s_message\"\n", id))
        strings.write_string(b, "            android:layout_width=\"wrap_content\"\n")
        strings.write_string(b, "            android:layout_height=\"wrap_content\"\n")
        strings.write_string(b, "            android:text=\"Something went wrong\" />\n\n")
        strings.write_string(b, fmt.tprintf("        <Button\n            android:id=\"@+id/%s_retry\"\n", id))
        strings.write_string(b, "            android:layout_width=\"wrap_content\"\n")
        strings.write_string(b, "            android:layout_height=\"wrap_content\"\n")
        strings.write_string(b, "            android:text=\"Retry\" />\n\n")
    case "content":
        for &w in s.layout.children {
            write_widget(b, &w, 2)
        }
    }
    strings.write_string(b, "    </LinearLayout>\n\n")
}

@(private)
write_widget :: proc(b: ^strings.Builder, w: ^parser.Widget, indent: int) {
    pad := strings.repeat("    ", indent)
    defer delete(pad)
    tag := "View"
    if w.type == "text" do tag = "TextView"
    else if w.type == "button" do tag = "Button"
    else if w.type == "input" do tag = "EditText"
    else if w.type == "image" do tag = "ImageView"
    else if w.type == "spacer" do tag = "Space"
    else if w.type == "column" || w.type == "row" do tag = "LinearLayout"

    strings.write_string(b, pad)
    strings.write_string(b, fmt.tprintf("<%s\n", tag))
    if w.id != "" {
        strings.write_string(b, pad)
        strings.write_string(b, fmt.tprintf("    android:id=\"@+id/%s\"\n", w.id))
    }
    width := "wrap_content"
    height := "wrap_content"
    if w.width != "" do width = w.width
    if w.height != "" do height = w.height
    strings.write_string(b, pad)
    strings.write_string(b, fmt.tprintf("    android:layout_width=\"%s\"\n", width))
    strings.write_string(b, pad)
    strings.write_string(b, fmt.tprintf("    android:layout_height=\"%s\"", height))
    if w.text != "" {
        strings.write_string(b, "\n")
        strings.write_string(b, pad)
        strings.write_string(b, fmt.tprintf("    android:text=\"%s\"", w.text))
    }
    strings.write_string(b, " />\n\n")
}

@(private)
generate_screen_base :: proc(model: parser.AppModel, s: parser.Screen, class_name: string) -> string {
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    strings.write_string(&b, "// asstudio:generated v1 DO NOT EDIT\n")
    strings.write_string(&b, fmt.tprintf("package %s;\n\n", model.app.pckg))
    strings.write_string(&b, "import android.os.Bundle;\n")
    strings.write_string(&b, "import android.view.View;\n")
    strings.write_string(&b, "import android.widget.Button;\n")
    strings.write_string(&b, "import android.widget.TextView;\n")
    strings.write_string(&b, "import androidx.appcompat.app.AppCompatActivity;\n\n")
    strings.write_string(&b, fmt.tprintf("public abstract class %sScreenBase extends AppCompatActivity {\n", class_name))

    for w in s.layout.children {
        if w.id == "" do continue
        jt := "View"
        if w.type == "text" do jt = "TextView"
        if w.type == "button" do jt = "Button"
        strings.write_string(&b, fmt.tprintf("    protected %s %s;\n", jt, w.id))
    }
    if len(s.states) > 0 {
        for st in s.states {
            field := camel(fmt.tprintf("%s_state_%s", s.id, st))
            strings.write_string(&b, fmt.tprintf("    private View %s;\n", field))
        }
    }
    strings.write_string(&b, "\n")
    strings.write_string(&b, "    @Override\n")
    strings.write_string(&b, "    protected void onCreate(Bundle savedInstanceState) {\n")
    strings.write_string(&b, "        super.onCreate(savedInstanceState);\n")
    strings.write_string(&b, fmt.tprintf("        setContentView(R.layout.%s);\n\n", s.id))
    for w in s.layout.children {
        if w.id == "" do continue
        strings.write_string(&b, fmt.tprintf("        %s = findViewById(R.id.%s);\n", w.id, w.id))
    }
    if len(s.states) > 0 {
        for st in s.states {
            field := camel(fmt.tprintf("%s_state_%s", s.id, st))
            rid := fmt.tprintf("%s_state_%s", s.id, st)
            strings.write_string(&b, fmt.tprintf("        %s = findViewById(R.id.%s);\n", field, rid))
        }
    }
    strings.write_string(&b, "\n")
    for w in s.layout.children {
        if w.on_click != "" && w.id != "" {
            method := strings.concatenate({"on", capitalize(w.on_click)})
            strings.write_string(&b, fmt.tprintf("        %s.setOnClickListener(v -> %s());\n", w.id, method))
        }
    }
    for st in s.states {
        if st == "error" {
            rid := fmt.tprintf("%s_state_error_retry", s.id)
            strings.write_string(&b, fmt.tprintf("        findViewById(R.id.%s).setOnClickListener(v -> onRetry());\n", rid))
        }
    }
    if len(s.states) > 0 {
        strings.write_string(&b, fmt.tprintf("\n        setState(\"%s\");\n", s.states[0]))
    }
    strings.write_string(&b, "    }\n\n")
    for w in s.layout.children {
        if w.on_click != "" {
            method := strings.concatenate({"on", capitalize(w.on_click)})
            strings.write_string(&b, fmt.tprintf("    protected abstract void %s();\n\n", method))
        }
    }
    if len(s.states) > 0 {
        strings.write_string(&b, "    public void setState(String state) {\n")
        for st in s.states {
            field := camel(fmt.tprintf("%s_state_%s", s.id, st))
            strings.write_string(&b, fmt.tprintf("        %s.setVisibility(\"%s\".equals(state) ? View.VISIBLE : View.GONE);\n", field, st))
        }
        strings.write_string(&b, "    }\n")
    }
    strings.write_string(&b, "}\n")
    return strings.clone(strings.to_string(b))
}

@(private)
generate_activity :: proc(model: parser.AppModel, s: parser.Screen, class_name: string) -> string {
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    strings.write_string(&b, fmt.tprintf("package %s;\n\n", model.app.pckg))
    strings.write_string(&b, "// User-owned file: asstudio never overwrites it once it exists.\n")
    strings.write_string(&b, "// Handler bodies are yours; the base class is regenerated around them.\n")
    strings.write_string(&b, fmt.tprintf("public class %sActivity extends %sScreenBase {\n", class_name, class_name))
    for w in s.layout.children {
        if w.on_click != "" {
            method := strings.concatenate({"on", capitalize(w.on_click)})
            strings.write_string(&b, fmt.tprintf("    @Override\n    protected void %s() {\n        // TODO: implement %s()\n    }\n", method, method))
        }
    }
    strings.write_string(&b, "}\n")
    return strings.clone(strings.to_string(b))
}

@(private)
screen_class_name :: proc(id: string) -> string {
    return capitalize(id)
}

@(private)
package_to_path :: proc(pkg: string) -> string {
    pckgd, _ := strings.replace_all(pkg, ".", "/")
    return pckgd
}

@(private)
capitalize :: proc(s: string) -> string {
    if len(s) == 0 do return s
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    first := true
    for r in s {
        if first {
            strings.write_rune(&b, unicode.to_upper(r))
            first = false
        } else {
            strings.write_rune(&b, r)
        }
    }
    return strings.clone(strings.to_string(b))
}

@(private)
camel :: proc(s: string) -> string {
    parts := strings.split(s, "_")
    defer delete(parts)
    if len(parts) == 0 do return s
    b: strings.Builder
    strings.builder_init(&b)
    defer strings.builder_destroy(&b)
    strings.write_string(&b, parts[0])
    for i := 1; i < len(parts); i += 1 {
        strings.write_string(&b, capitalize(parts[i]))
    }
    return strings.clone(strings.to_string(b))
}
