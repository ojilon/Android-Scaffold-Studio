# JNI JSON Bridge (Design Only - Phase 1)

## Pattern
Java sends only a **request string plus an output path** across JNI; native code does the work and writes a **JSON file** to that path; Java reads the file.

## Benefits
- Tiny JNI surface
- Easy to template
- Easy to test on desktop

## Implementation
The JNI glue is a thin C ABI (`jni_bridge.c` plus a C header). Backend logic sits behind that header and can be **C first, then Odin or Zig** as optional "flavor" templates (Odin/Zig struggle with bionic on Android 10 and below, so keep the C ABI boundary).

## Template Stub
Add a template stub folder `templates/cpp/jni_json_bridge/` with a README describing the planned files; do not implement in Phase 1.
