# JNI

The design is in **[05-jni-json-bridge.md](05-jni-json-bridge.md)**; the Phase 2
stub and its file plan are in `templates/cpp/jni_json_bridge/README.md`.

One sentence: Java sends a request string plus an output path across a thin C ABI,
native code writes a JSON file to that path, Java reads the file. The backend can
later be Odin or Zig without changing the boundary.
