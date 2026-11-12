# WayDirect patterns captured for Scaffold Studio

WayDirect (https://github.com/ojilon/WayDirect) proved a working hybrid on a
real device:

- **UI** – Material 3 Java + XML (Fragments / NavigationBarView / ViewBinding)
- **Native engine** – Odin + thin bionic-safe C only where the NDK forces it
- **JNI** – pure Odin using official Android bindings
- **Large results** – never cross JNI as big strings; native writes under the
  app cache / private home and returns `{status, path}`

This branch turns those practices into reusable templates so every generated
app starts with the same disciplined structure.

## 1. Screen states

Every screen that declares `"states": ["loading","content","error"]` gets:

- A root `FrameLayout`
- Three child containers with stable ids (`<screen>_state_loading` etc.)
- A generated `setState(String)` that shows exactly one
- Loading = centred ProgressBar
- Error = message + retry button (wired to the same handler as the content
  retry button)

Template: `templates/xml/screen_with_states.xml.in`  
Generator: `codegen.generate_layout_xml`

## 2. Generated vs user-owned Java

| File | Policy |
|------|--------|
| `*ScreenBase.java` | Always overwritten (marker comment) |
| `*Activity.java`   | Written only if missing |

Handlers become `protected abstract void onXxx()` on the base; the user
implements them in the Activity. Regenerating the base never destroys user
logic.

Templates: `templates/java/ScreenBase.java.in`, `Activity.java.in`

## 3. File-backed JNI results

```
Java                          Odin / native
────                          ────────────
request_json  ─────────────►  parse (small)
out_path      ─────────────►  do work under app_home / cache
                              write large result to a file
                              write tiny JSON {status,path} to out_path
◄──────────────────────────── read out_path, then open the payload file
```

Benefits:
- Tiny JNI surface
- Easy to unit-test on desktop
- No Binder transaction size surprises
- Same pattern for every action

Template: `templates/jni/file_result_bridge.odin.in`

## 4. Odin engine core

- One action-id router
- `set_app_home` called once from Java (`Context.getFilesDir()`)
- Domain modules (storage, documents, media …) live under the engine package
- Remaining C is only bionic-safe wrappers (opendir / readdir / stat …)

Template: `templates/odin/engine_core.odin.in`

## 5. Gradle stays in charge of the APK

- Odin builds `lib….so` (script under `native/build-android.*`)
- Gradle packages Java + resources + the pre-built `.so` from `jniLibs/`
- ABI filter defaults to `arm64-v8a` (matching the WayDirect experiment branch)

Templates under `templates/gradle/` will be filled in the next pass.

## 6. What still needs to be templated later

- Material 3 theme / colour tokens / typography
- Bottom navigation / NavigationBarView shell
- ViewBinding-enabled Fragment hosts
- Chart / list / detail screen patterns
- Common button styles and ripple treatment
- Logcat tag conventions (`WayDirectNative`, `WayDirectBridge` …)

Each of these can be added as a new template family without changing the
core pipeline.
