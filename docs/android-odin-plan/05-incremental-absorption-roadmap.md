# 05 — Incremental absorption roadmap

Goal: absorb more of `core/sys/android` and shift core logic/state from Java into Odin, one tested milestone at a time. Do not move to the next milestone until the previous one builds, installs, and runs on a device/emulator.

## Milestone 1 — Odin `.so` compiles and loads (Step 02)

- [ ] `tests/odinandroid/main.odin` compiles with `-target:linux_arm64 -subtarget:android -build-mode:shared`.
- [ ] `lib/arm64-v8a/libhello.so` has `native_hello` exported and expected `NEEDED` libs (`liblog`, `libandroid`, `libc`).
- [ ] `.so` is placed at `generated/hello/lib/arm64-v8a/libengine.so` and bundled into an APK.
- [ ] APK installs and launches `HomeActivity` without an unsatisfied-link error.

**Test:** `adb logcat` shows "Odin says: ..." or `native_hello` symbol present.

---

## Milestone 2 — Java ↔ Odin JNI bridge works (Step 03)

- [ ] `JNI_OnLoad` + `RegisterNatives` registers `setAppHome`, `handleAction`, `uiSetTitleText` on `com.oji.hello.NativeBridge`.
- [ ] Java calls `NativeBridge.handleAction(ACTION_PING, ...)` and Odin writes a `{status,path}` result file.
- [ ] Odin calls back `HomeActivity.setTitleText(...)` via the global Activity ref and the title changes on screen.
- [ ] `HomeScreenBase` still extends `android.app.Activity` (no androidx/AAR yet).

**Test:** Tap Retry → logcat shows Odin handling the action → title text updates.

---

## Milestone 3 — Full APK pipeline in the CLI (Step 04)

- [ ] `asstudio build-apk <model.json> -o out.apk` runs the whole pipeline: `.so` → `aapt2` → `javac` → `d8` → package → `zipalign` → `apksigner`.
- [ ] `apksigner verify out.apk` passes.
- [ ] `adb install -r out.apk` + `am start` works.
- [ ] Debug keystore creation is handled when `~/.android/debug.keystore` is missing.

**Test:** One command produces an installable, signed APK from `samples/hello.json`.

---

## Milestone 4 — Asset loading moves into Odin (`core:sys/android/assets`)

- [ ] Ship a file in `generated/hello/assets/`.
- [ ] Odin reads it with `asset_read_entire_file("hello.txt")` instead of Java `AssetManager`.
- [ ] `asset_list("")` enumerates the assets directory.

**Test:** Odin logs the asset contents at startup.

---

## Milestone 5 — App-home / private storage owned by Odin

- [ ] Java calls `NativeBridge.setAppHome(getFilesDir().getAbsolutePath())` once in `onCreate`.
- [ ] Odin stores `g_app_home` and writes large JNI results there (file-backed pattern from `templates/jni/file_result_bridge.odin.in`).
- [ ] No big strings cross JNI; only `{status, path}` JSON does.

**Test:** A `handle_action` writes a payload file under the app home and returns its path.

---

## Milestone 6 — Logging moved to Odin (`core:sys/android/log` + `stdio_logcat`)

- [ ] Odin uses `__android_log_write(.INFO, "OdinEngine", ...)` directly instead of relying on `fmt.println` → logcat redirection only.
- [ ] `redirect_stdio_to_logcat` still active as a fallback for stdout/stderr.

**Test:** Logcat shows Odin-tagged lines separate from `fmt.println` output.

---

## Milestone 7 — Activity lifecycle handled by Odin glue (`native_app_glue`)

- [ ] The Odin `.so` uses `ANativeActivity_onCreate` via `core:sys/android/native_app_glue` so `main()` runs on Android.
- [ ] `onAppCmd` handles `APP_CMD_INIT_WINDOW`, `APP_CMD_RESUME`, `APP_CMD_PAUSE`, `APP_CMD_DESTROY`.
- [ ] `onInputEvent` handles touch/key input.

**Test:** Rotate the device; app survives (`APP_CMD_CONFIG_CHANGED`) and input events are logged.

---

## Milestone 8 — Shift screen logic Java → Odin (state machine)

- [ ] `setState(loading/content/error)` is driven from Odin via `uiSetState` JNI callback instead of only Java-side.
- [ ] Action router in `engine_core.odin` owns the screen's business logic; Java stays a thin view layer.

**Test:** Trigger a state change from Odin; the XML views update accordingly.

---

## Milestone 9 — Second ABI + release signing

- [ ] Build `x86_64` (emulator) in addition to `arm64-v8a`.
- [ ] Replace debug keystore with a release keystore (`-android-keystore`, `-android-keystore-alias`, `-android-keystore-password`).
- [ ] Produce an `.aab` via `odin bundle android ... -out:out.aab` (bundletool path).

**Test:** Emulator (x86_64) and device (arm64-v8a) both install and run.

---

## Milestone 10 — Material 3 / androidx (after the above are stable)

- [ ] Add AAR resolution to the CLI for `androidx.appcompat:appcompat` and Material 3.
- [ ] `HomeScreenBase` switches to `AppCompatActivity`.
- [ ] Theme/color/typography templates under `templates/android/themes.xml.in`.

**Test:** App uses Material 3 widgets and theming; `AppCompatActivity` features work.

---

## Rule for every milestone

> Build → install → run → check `adb logcat` before moving on. If a milestone fails on device, fix it there; do not pile the next milestone on top.
