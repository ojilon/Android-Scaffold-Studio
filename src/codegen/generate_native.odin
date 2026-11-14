package codegen

import "src:parser"
import "core:fmt"
import "core:strings"

// Emits Odin native + file-backed JNI bridge when any screen requests a backend.
// Pattern from WayDirect: small request in, large result via file path.

any_native_backend :: proc(model: parser.AppModel) -> bool {
    for s in model.screens {
        if s.backend == "odin_jni" || s.backend == "jni_json_bridge" {
            return true
        }
    }
    return false
}

append_native_engine :: proc(files: ^[dynamic]GeneratedFile, model: parser.AppModel) {
    if !any_native_backend(model) do return

    pkg_odin := "engine"

    bridge := strings.concatenate({
        "// asstudio:generated v1 DO NOT EDIT\n",
        "// File-backed JNI result pattern (WayDirect).\n\n",
        "package ", pkg_odin, "\n\n",
        "import \"core:c\"\n",
        "import \"core:fmt\"\n",
        "import \"core:os\"\n",
        "import \"core:strings\"\n\n",
        "STATUS_OK           :: 0\n",
        "STATUS_ERR_IO       :: 1\n",
        "STATUS_ERR_ARGS     :: 2\n",
        "STATUS_ERR_INTERNAL :: 3\n\n",
        "write_result_ok :: proc(out_path: string, payload_path: string) -> c.int {\n",
        "    content := fmt.tprintf(`{\"status\":0,\"path\":\"%s\"}`, payload_path)\n",
        "    if !os.write_entire_file(out_path, transmute([]byte)content) do return STATUS_ERR_IO\n",
        "    return STATUS_OK\n",
        "}\n\n",
        "write_result_err :: proc(out_path: string, status: c.int, message: string) -> c.int {\n",
        "    content := fmt.tprintf(`{\"status\":%d,\"message\":\"%s\"}`, status, message)\n",
        "    if !os.write_entire_file(out_path, transmute([]byte)content) do return STATUS_ERR_IO\n",
        "    return status\n",
        "}\n\n",
        "ACTION_PING      :: 1\n",
        "ACTION_LIST_DIR  :: 2\n",
        "ACTION_READ_FILE :: 3\n\n",
        "@(private)\n",
        "g_app_home: string\n\n",
        "set_app_home :: proc(path: string) {\n",
        "    if g_app_home != \"\" do delete(g_app_home)\n",
        "    g_app_home = strings.clone(path)\n",
        "}\n\n",
        "handle_action :: proc(action: c.int, request_json: string, out_path: string) -> c.int {\n",
        "    _ = request_json\n",
        "    switch action {\n",
        "    case ACTION_PING:\n",
        "        return write_result_ok(out_path, \"\")\n",
        "    case ACTION_LIST_DIR:\n",
        "        return write_result_err(out_path, STATUS_ERR_INTERNAL, \"list_dir not implemented\")\n",
        "    case ACTION_READ_FILE:\n",
        "        return write_result_err(out_path, STATUS_ERR_INTERNAL, \"read_file not implemented\")\n",
        "    }\n",
        "    return write_result_err(out_path, STATUS_ERR_ARGS, fmt.tprintf(\"unknown action %d\", action))\n",
        "}\n",
    })

    append(files, GeneratedFile{
        path    = "native/engine.odin",
        content = bridge,
    })

    java_bridge := strings.concatenate({
        "// asstudio:generated v1 DO NOT EDIT\n",
        "package ", model.app.pckg, ";\n\n",
        "/** Thin JNI facade. Large results via file path, not JNI buffers. */\n",
        "public final class NativeBridge {\n",
        "    static {\n",
        "        System.loadLibrary(\"engine\");\n",
        "    }\n\n",
        "    public static final int ACTION_PING = 1;\n",
        "    public static final int ACTION_LIST_DIR = 2;\n",
        "    public static final int ACTION_READ_FILE = 3;\n\n",
        "    public static native void setAppHome(String path);\n",
        "    public static native int handleAction(int action, String requestJson, String outPath);\n",
        "}\n",
    })

    pkg_path, _ := strings.replace_all(model.app.pckg, ".", "/")
    append(files, GeneratedFile{
        path    = fmt.tprintf("java/%s/NativeBridge.java", pkg_path),
        content = java_bridge,
    })
}
