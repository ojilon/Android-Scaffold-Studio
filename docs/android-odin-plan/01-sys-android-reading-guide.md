# 01 — Reading guide: `core/sys/android` (Odin NDK bindings)

Goal: learn how to navigate `D:\Dev\Odin\core\sys\android` without drowning in the ~878 declarations across 43 files. Every file is generated from NDK r30 headers by `core/sys/android/_gen/generate.py` and starts with a `DO NOT EDIT` marker naming its source header.

## How to navigate (start here, then expand)

Do not read the package top-to-bottom. Pick a concern, read its 1–3 files, then stop.

| Concern | Files to read first | What you get |
|---|---|---|
| JNI types + the `JNIEnv` function table | `jni.odin` | `jobject`, `jstring`, `jint`, `JNIEnv`, `JNINativeInterface`, `JNI_GetDefaultJavaVMInitArgs` / `JNI_CreateJavaVM` |
| Implicit per-thread `JNIEnv` (attach/detach, VM storage) | `jni_env.odin` | `jni_set_vm`, `jni_vm`, `jni_env`, `jni_take_exception` |
| Logcat output | `log.odin` | `android_LogPriority`, `__android_log_write`, `__android_log_print` |
| Assets from the APK | `asset_manager.odin` + `assets.odin` | `AAssetManager`, `AAssetManager_fromJava`, `asset_read_entire_file`, `asset_to_stream`, `asset_list` |
| Activity / window / input lifecycle | `native_app_glue/native_app_glue.odin` | `app()`, `android_app`, `APP_CMD_*`, `ALooper_pollOnce` loop |
| Redirect stdout/stderr to logcat | `stdio_logcat.odin` | `redirect_stdio_to_logcat` |

Read in this order for your first pass:

1. `jni.odin` — copy the `jvalue`, `JNIEnv`, `JavaVM`, `JNINativeMethod` shapes into your head. These are the C ABI you will call from Odin.
2. `jni_env.odin` — understand `jni_set_vm` (called once by `native_app_glue` before `main`) and `jni_env` (attaches the calling thread on first use).
3. `log.odin` — `android_LogPriority` enum + `__android_log_write` / `__android_log_print`. This is your debug channel on device.
4. `asset_manager.odin` + `assets.odin` — `asset_read_entire_file(path) -> ([]byte, bool)` is the primitive for reading `assets/` at runtime.
5. `native_app_glue/native_app_glue.odin` — only when you need the `NativeActivity` entry point (`ANativeActivity_onCreate`) and the `APP_CMD_*` loop.

## Grouped starting points

### JNI
Types: `jboolean u8`, `jbyte i8`, `jchar u16`, `jint i32`, `jlong i64`, `jsize jint`, `jobject rawptr`, `jclass jobject`, `jstring jobject`, `jthrowable jobject`.
Handles: `JNIEnv` (struct with `using functions: ^JNINativeInterface`), `JavaVM` (same shape for `JNIInvokeInterface`).
Helpers: `jni_set_vm(vm)`, `jni_env()`, `jni_take_exception(env)`.
Call pattern (from `vendor:android/java`):
```odin
jni_env, _, jni_id := jni_method(&cache, &Class__class, "methodName", "(sig)V", false) or_return
jni_args := [?]android.jvalue{{i = value}}
jni_env->CallVoidMethodA(self, jni_id, &jni_args[0])
return jni_take(jni_env)
```

### Logging / logcat
`android_LogPriority :: enum c.int { UNKNOWN, DEFAULT, VERBOSE, DEBUG, INFO, WARN, ERROR, FATAL, SILENT }`.
Procs: `__android_log_write(prio, tag, text)`, `__android_log_print(prio, tag, fmt, ..args)`.
`redirect_stdio_to_logcat(tag)` pipes stdout (INFO) and stderr (WARN) to logcat; called by `native_app_glue` before `main`.

### Assets
`AAssetManager :: struct {}`, `AAsset :: struct {}`, `AAssetMode :: enum i32 { UNKNOWN, RANDOM, STREAMING, BUFFER }`.
Key proc: `AAssetManager_fromJava(env: ^JNIEnv, assetManager: jobject) -> ^AAssetManager`.
Odin helpers: `asset_read_entire_file(path) -> ([]byte, bool)`, `asset_to_stream(asset) -> io.Stream`, `asset_list(path) -> ([]string, bool)`.

## What to verify in the local Odin compiler build

Before writing code against these bindings, confirm:

1. `odin version` reports `dev-2026-11` (or compatible).
2. `ODIN_ANDROID_NDK` points to NDK r30 (`D:\Dev\AndroidCLI\sdk\ndk\30.0.16248370` here) — the bindings were generated from r30 headers.
3. `ODIN_ANDROID_SDK` points to a SDK with build-tools + `platforms/android-36` (`D:\Dev\AndroidCLI\sdk` here).
4. `JAVA_HOME` is set (`D:\Dev\AndroidCLI\openjdk` here).
5. A minimal `odin build ... -target:linux_arm64 -subtarget:android -build-mode:shared` compiles a shared lib linking `liblog`, `libandroid`, `libnativewindow` without missing symbols.

## Why `HomeScreenBase.java` should start with `android.app.Activity`

The golden file currently extends `AppCompatActivity` from `androidx.appcompat`. For the Odin-only path (no Gradle, no AAR resolution), do not depend on external libraries yet.

`android.app.Activity` is in `android.jar`, which the Odin bundler already resolves via `--min-sdk-version` / `--target-sdk-version` against the local platform (`platforms/android-36/android.jar`). `AppCompatActivity` lives in an external AAR that would require:

- resolving `androidx.appcompat:appcompat` from Google Maven,
- adding its AAR to `-I` / compile classpath,
- shipping its resources into the APK.

That is Gradle/AAPT2 dependency resolution the CLI does not implement yet. Starting with `android.app.Activity` keeps the first working APK self-contained: only `android.jar` + your Odin `.so` + `classes.dex`. Move to `AppCompatActivity`/Material 3 only after Step 04 (APK pipeline) is proven, by adding an explicit AAR-resolution step to the CLI.
