# 04 — APK build & sign pipeline (Odin orchestrator)

Goal: implement the APK pipeline inside the Odin CLI using `os.process_exec` (or `os2.process_exec` in your codebase) — no Gradle. Steps: build `.so`, compile resources with `aapt2`, compile Java with `javac`, dex with `d8`, package, align, sign, verify, install.

## Environment

- `ODIN_ANDROID_SDK=D:\Dev\AndroidCLI\sdk`
- `ODIN_ANDROID_NDK=D:\Dev\AndroidCLI\sdk\ndk\30.0.16248370`
- `JAVA_HOME=D:\Dev\AndroidCLI\openjdk`
- `adb` on PATH (or `ODIN_ANDROID_SDK/platform-tools/adb.exe`)

## Directory layout consumed by the pipeline

```
generated/hello/                 # codegen output root (app dir for odin bundle android)
  AndroidManifest.xml
  res/layout/home.xml
  res/values/strings.xml
  assets/                        # optional
  lib/arm64-v8a/libengine.so     # built by step 02
  java/com/oji/hello/*.java      # compiled to classes.dex
```

## Step-by-step commands (run these manually first)

### a) Compile Odin `.so` (arm64-v8a)

```powershell
mkdir -p generated/hello/lib/arm64-v8a
D:\Dev\Odin\odin.exe build src/engine `
	-target:linux_arm64 -subtarget:android -minimum-os-version:29 -build-mode:shared `
	-out:generated/hello/lib/arm64-v8a/libengine.so
```

### b) Compile XML/resources with `aapt2`

```powershell
$SDK = "D:\Dev\AndroidCLI\sdk"
$BT = "$SDK\build-tools\37.0.0"
$PLATFORM = "$SDK\platforms\android-36"
$APP = "generated/hello"
$WORK = "$APP\.build"

mkdir $WORK
& "$BT\aapt2.exe" compile --dir "$APP\res" -o "$WORK\res.zip"
& "$BT\aapt2.exe" link -o "$WORK\linked.zip" `
	-I "$PLATFORM\android.jar" `
	--manifest "$APP\AndroidManifest.xml" `
	-R "$WORK\res.zip" --auto-add-overlay `
	-A "$APP\assets" `
	--min-sdk-version 29 --target-sdk-version 36 `
	--version-code 1 --version-name 1.0
```

### c) Compile Java with `javac`

```powershell
$JDK = "D:\Dev\AndroidCLI\openjdk"
$JAVA_SRC = "$APP\java"
$CLASSES = "$WORK\classes"
mkdir $CLASSES

# -bootclasspath against android.jar so androidx is NOT resolved yet (step 01 rationale)
& "$JDK\bin\javac.exe" -bootclasspath "$PLATFORM\android.jar" --release 8 `
	-d "$CLASSES" $(Get-ChildItem "$JAVA_SRC" -Recurse -Filter *.java | ForEach-Object { $_.FullName })
```

### d) Convert `.class` to `classes.dex` with `d8`

```powershell
& "$BT\d8.exe" --release --output "$WORK" "$CLASSES"
# produces $WORK\classes.dex
```

### e) Package into APK (store/uncompressed .so + classes.dex), align, sign

Odin's own `bundle_command.cpp` already does the zip part; replicate or call `odin bundle android`:

```powershell
# Option A: use Odin's bundler (recommended; it also adds vendor/classes.dex)
D:\Dev\Odin\odin.exe bundle android generated/hello `
	-minimum-os-version:29 `
	-out:generated/hello.apk

# Option B: manual zip + zipalign + apksigner (if you implement your own packager)
#   zip -0 generated/hello.apk classes.dex lib/arm64-v8a/libengine.so AndroidManifest.xml res/...
#   "$BT\zipalign.exe" -p -f 4 generated/hello.apk generated/hello.aligned.apk
```

**Debug keystore** (create once if missing):

```powershell
$HOME = $env:USERPROFILE
$KS = "$HOME\.android\debug.keystore"
if (!(Test-Path $KS)) {
	mkdir "$HOME\.android" -ErrorAction SilentlyContinue
	& "$JDK\bin\keytool.exe" -genkeypair -keystore $KS -alias androiddebugkey `
		-storepass android -keypass android -keyalg RSA -validity 10000 `
		-dname "CN=Android Debug,O=Android,C=US"
}
```

**Sign**:

```powershell
& "$BT\apksigner.bat" sign --ks $KS --ks-key-alias androiddebugkey --ks-pass pass:android generated/hello.apk
```

**Verify**:

```powershell
& "$BT\apksigner.bat" verify generated/hello.apk
```

### f) Install & run

```powershell
adb install -r generated/hello.apk
adb shell am start -n com.oji.hello/com.oji.hello.HomeActivity
adb logcat -s OdinEngine com.oji.hello
```

## Odin orchestrator code pattern

Add a `build_apk` procedure to `src/app/main.odin` (or a new `src/app/android_build.odin`) using `os.process_exec`:

```odin
package main

import "core:os"
import "core:fmt"
import "core:path/filepath"
import "core:strings"

android_build :: proc(model_dir: string, out_apk: string) {
	sdk := os.get_env("ODIN_ANDROID_SDK") or_return  // helper you write
	bt  := filepath.join({sdk, "build-tools"})   // pick newest subdir
	platform := filepath.join({sdk, "platforms", "android-36"})
	jnk   := filepath.join({model_dir, ".build"})

	os.make_directory_all(jnk)

	// a) .so (already built by caller or here)
	// b) aapt2 compile
	os.process_exec(
		exe = filepath.join({bt, "aapt2"}),
		args = ["compile", "--dir", filepath.join({model_dir, "res"}), "-o", filepath.join({jnk, "res.zip"})],
	)
	// c) aapt2 link
	os.process_exec(
		exe = filepath.join({bt, "aapt2"}),
		args = ["link", "-o", filepath.join({jnk, "linked.zip")],
			"-I", filepath.join({platform, "android.jar"}),
			"--manifest", filepath.join({model_dir, "AndroidManifest.xml"}),
			"-R", filepath.join({jnk, "res.zip"}), "--auto-add-overlay",
			"--min-sdk-version", "29", "--target-sdk-version", "36",
			"--version-code", "1", "--version-name", "1.0"],
	)
	// d) javac, d8, zip, zipalign, apksigner ... same pattern
	// e) odin bundle android (call odin itself)
	os.process_exec(
		exe = "odin",
		args = ["bundle", "android", model_dir, "-minimum-os-version:29", "-out", out_apk],
	)
}
```

`os.process_exec` returns the exit code; propagate non-zero as a build failure. Capture stdout/stderr if you want to surface `aapt2`/`javac` errors.

## What to verify before declaring Step 04 done

1. `lib/arm64-v8a/libengine.so` exists after the Odin build.
2. `aapt2 link` produces `linked.zip` containing `AndroidManifest.xml`, `resources.arsc`, and `res/`.
3. `javac` produces `.class` files under `.build/classes` with no `androidx` unresolved-symbol errors.
4. `d8` produces `classes.dex`.
5. `odin bundle android ... -out:hello.apk` produces a zip whose `classes.dex` and `lib/arm64-v8a/libengine.so` are stored (uncompressed).
6. `apksigner verify` passes.
7. `adb install` + `am start` launches `HomeActivity` and logcat shows Odin output.

## Next step after this file

Once the pipeline produces an installable, signed APK that runs `HomeActivity`, proceed to step 05 for the incremental roadmap (absorb more of `core/sys/android`, shift logic Java → Odin).
