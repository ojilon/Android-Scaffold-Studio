# 02 — Compile an Odin shared library for Android (`libhello.so`)

Goal: produce `lib/arm64-v8a/libhello.so` from an Odin package using only `odin build` + the NDK toolchain. No Gradle. Verify compilation first with a minimal snippet.

## Prerequisites (already present in this environment)

- `ODIN_ANDROID_NDK=D:\Dev\AndroidCLI\sdk\ndk\30.0.16248370`
- `ODIN_ANDROID_SDK=D:\Dev\AndroidCLI\sdk` (build-tools 34–37, platform `android-36`)
- `JAVA_HOME=D:\Dev\AndroidCLI\openjdk`

## Minimal test snippet

Create a scratch package so you can compile before wiring the real generator output.

`tests/odinandroid/main.odin`:
```odin
package main

import "core:fmt"
import "core:os"

// Called from Java via JNI_OnLoad / RegisterNatives (see step 03).
export
native_hello :: proc "c" (message: cstring) -> cint {
	fmt.println("Odin says:", message)
	return 0
}

main :: proc() {
	// On Android this is never reached directly; NativeActivity drives via ANativeActivity_onCreate.
	fmt.println("native lib loaded")
}
```

## Compile command (arm64-v8a)

```powershell
# From the repo root
mkdir -p build/ndk/arm64-v8a
D:\Dev\Odin\odin.exe build tests/odinandroid `
	-target:linux_arm64 `
	-subtarget:android `
	-minimum-os-version:29 `
	-build-mode:shared `
	-out:build/ndk/arm64-v8a/libhello.so
```

What each flag means:
- `-target:linux_arm64` — ARM64 Linux target (Android uses the Linux ARM64 backend).
- `-subtarget:android` — selects Android sysroot/libraries and the `ANativeActivity_onCreate` startup path.
- `-minimum-os-version:29` — min API level; Odin requires >= 29.
- `-build-mode:shared` — emits a `.so`, not an executable.
- `-out:...` — destination path; Odin does **not** create parent directories, so `mkdir` first.

Repeat for other ABIs by swapping target/out dir:
- `armeabi-v7a`: `-target:linux_arm32 -out:.../armeabi-v7a/libhello.so`
- `x86`: `-target:linux_i386 -out:.../x86/libhello.so`
- `x86_64`: `-target:linux_amd64 -out:.../x86_64/libhello.so`

## What Odin links automatically

When `-subtarget:android` is set, Odin links the NDK stub libraries for the bindings you import:
- `liblog` (`core:sys/android/log.odin`)
- `libandroid` (`core:sys/android/asset_manager.odin`, `native_window.odin`)
- `libnativewindow` (`core:sys/android/native_window.odin`)
- `libnativehelper` (`core:sys/android/jni.odin` — `JNI_CreateJavaVM` etc.)
- `libc` / bionic

You do **not** pass `-llog` manually unless you add a raw `foreign import` yourself.

## Verify it compiled

```powershell
# 1. File exists and is non-empty
ls -l build/ndk/arm64-v8a/libhello.so

# 2. Inspect dynamic symbols (NDK readelf/nm)
&D:\Dev\AndroidCLI\sdk\ndk\30.0.16248370\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android-readelf.exe -s build/ndk/arm64-v8a/libhello.so | Select-String "native_hello"

# 3. Confirm it depends on Android system libs
&D:\Dev\AndroidCLI\sdk\ndk\30.0.16248370\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android-readelf.exe -d build/ndk/arm64-v8a/libhello.so | Select-String "NEEDED"
```

Expected `NEEDED` entries: `liblog.so`, `libandroid.so`, `libnativewindow.so`, `libc.so`, `libdl.so`.

## Next step after this file

Once `libhello.so` builds cleanly, proceed to step 03 to connect it to `HomeScreenBase.java` via JNI, then step 04 to package everything into an APK.
