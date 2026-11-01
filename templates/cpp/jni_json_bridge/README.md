# jni_json_bridge (Phase 2 stub)

Planned files, not implemented in Phase 1:

```
jni_bridge.c        thin C ABI: one exported function
jni_bridge.h        header the Java side and the backend share
bridge_backend.c    C backend behind the C ABI (first flavour)
README.md           this file
```

## Contract

Java passes a **request string** and an **output path** across JNI. Native code
does the work and writes a **JSON file** to that path; Java reads the file.

Nothing but that one call crosses the boundary, which keeps the JNI surface
tiny, makes the backend testable on the desktop, and lets the backend be
swapped (C now, Odin or Zig later) without touching Java.

Odin and Zig both struggle with bionic on Android 10 and below, which is why
the C ABI boundary stays even when the backend changes.

Design details: docs/05-jni-json-bridge.md.
