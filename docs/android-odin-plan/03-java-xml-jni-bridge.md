# 03 — Java/XML/JNI bridge: `HomeScreenBase` + `home.xml` ↔ Odin

Goal: connect the generated `com.oji.hello` Java/XML UI to the Odin native library using `JNI_OnLoad` + `RegisterNatives`, and let Odin call back into Java UI methods (`setState`, `setTitleText`). Package: `com.oji.hello`, screen: `home`.

## Layout assumption (from generator output)

```
generated/hello/
  AndroidManifest.xml
  res/layout/home.xml
  res/values/strings.xml
  java/com/oji/hello/HomeScreenBase.java   (generated, overwritten)
  java/com/oji/hello/HomeActivity.java     (user-owned, written once)
  native/
    jni_bridge.odin        (Odine JNI_OnLoad + RegisterNatives)
    engine_core.odin       (action router, set_app_home, handle_action)
  lib/arm64-v8a/libengine.so   (built in step 02/04)
```

## A) XML — `res/layout/home.xml`

Keep the state-view ids the generator already reserves (`home_state_*`). Add a title `TextView` and retry `Button`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<FrameLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:id="@+id/home_root"
    android:layout_width="match_parent"
    android:layout_height="match_parent">

    <View android:id="@+id/home_state_loading"
        android:layout_width="match_parent" android:layout_height="match_parent" />

    <LinearLayout android:id="@+id/home_state_content"
        android:layout_width="match_parent" android:layout_height="match_parent"
        android:orientation="vertical"
        android:gravity="center">

        <TextView android:id="@+id/title"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="Hello" />

        <Button android:id="@+id/retry"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="Retry" />
    </LinearLayout>

    <View android:id="@+id/home_state_error"
        android:layout_width="match_parent" android:layout_height="match_parent"
        android:visibility="gone" />

</FrameLayout>
```

## B) Java base class — `HomeScreenBase.java`

Extend `android.app.Activity` (step 01 rationale: no AAR resolution yet). Declare the native methods and wire the retry button to an action id the Odin side understands.

```java
// asstudio:generated v1 DO NOT EDIT
package com.oji.hello;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public abstract class HomeScreenBase extends android.app.Activity {
    protected TextView title;
    protected Button retry;
    private View homeStateLoading;
    private View homeStateContent;
    private View homeStateError;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.home);

        title = findViewById(R.id.title);
        retry = findViewById(R.id.retry);
        homeStateLoading = findViewById(R.id.home_state_loading);
        homeStateContent = findViewById(R.id.home_state_content);
        homeStateError = findViewById(R.id.home_state_error);

        retry.setOnClickListener(v -> onRetry());
        findViewById(R.id.home_state_error_retry).setOnClickListener(v -> onRetry());

        // Tell Odin the app home (private files dir) once.
        NativeBridge.setAppHome(getFilesDir().getAbsolutePath());
        setState("loading");
    }

    protected abstract void onRetry();

    public void setState(String state) {
        homeStateLoading.setVisibility("loading".equals(state) ? View.VISIBLE : View.GONE);
        homeStateContent.setVisibility("content".equals(state) ? View.VISIBLE : View.GONE);
        homeStateError.setVisibility("error".equals(state) ? View.VISIBLE : View.GONE);
    }

    // Called back from Odin via JNI.
    public void setTitleText(String text) {
        if (title != null) title.setText(text);
    }
}
```

`NativeBridge.java` (generated, thin JNI facade):

```java
// asstudio:generated v1 DO NOT EDIT
package com.oji.hello;

public final class NativeBridge {
    static {
        System.loadLibrary("engine");
    }

    public static final int ACTION_PING = 1;
    public static final int ACTION_RETRY = 2;

    public static native void setAppHome(String path);
    public static native int handleAction(int action, String requestJson, String outPath);

    // UI round-trip: Odin asks Java to update the screen.
    public static native void uiSetTitleText(String text);
}
```

## C) Odin native side

### `native/engine_core.odin` — action router

```odin
// asstudio:generated v1 DO NOT EDIT
package engine

import "core:c"
import "core:fmt"
import "core:os"
import "core:strings"

ACTION_PING  :: 1
ACTION_RETRY :: 2

STATUS_OK           :: 0
STATUS_ERR_IO       :: 1
STATUS_ERR_ARGS     :: 2
STATUS_ERR_INTERNAL :: 3

@(private)
g_app_home: string

set_app_home :: proc(path: string) {
	if g_app_home != "" do delete(g_app_home)
	g_app_home = strings.clone(path)
}

write_result_ok :: proc(out_path: string, payload_path: string) -> c.int {
	content := fmt.tprintf(`{"status":0,"path":"%s"}`, payload_path)
	if !os.write_entire_file(out_path, transmute([]byte)content) do return STATUS_ERR_IO
	return STATUS_OK
}

write_result_err :: proc(out_path: string, status: c.int, message: string) -> c.int {
	content := fmt.tprintf(`{"status":%d,"message":"%s"}`, status, message)
	if !os.write_entire_file(out_path, transmute([]byte)content) do return STATUS_ERR_IO
	return status
}

handle_action :: proc(action: c.int, request_json: string, out_path: string) -> c.int {
	switch action {
	case ACTION_PING:
		return write_result_ok(out_path, "")
	case ACTION_RETRY:
		// TODO: real retry logic
		return write_result_ok(out_path, "")
	}
	return write_result_err(out_path, STATUS_ERR_ARGS, fmt.tprintf("unknown action %d", action))
}
```

### `native/jni_bridge.odin` — `JNI_OnLoad` + `RegisterNatives`

This is the file the JVM calls when it loads `libengine.so`. It registers Odin procs as native methods of `com.oji.hello.NativeBridge` and holds a global ref to the running `HomeActivity` so Odin can call back (`uiSetTitleText`).

```odin
// asstudio:generated v1 DO NOT EDIT
package main

import "core:c"
import "core:fmt"
import "core:os"
import "core:strings"
import android "core:sys/android"
import engine "engine"

// ---------------------------------------------------------------------------
// JNI types (mirror core:sys/android/jni.odin)
jboolean      :: u8
jbyte         :: i8
jchar         :: u16
jint          :: i32
jlong         :: i64
jsize         :: jint
jobject       :: rawptr
jclass        :: jobject
jstring       :: jobject
jthrowable    :: jobject
JNI_FALSE     :: 0
JNI_TRUE      :: 1
JNI_VERSION_1_6 :: 0x00010006
JNI_OK        :: 0
JNI_ERR       :: -1

JNINativeMethod :: struct {
	name:      cstring
	signature: cstring
	fnPtr:     rawptr
}

JNIEnv :: struct {
	using functions: ^JNINativeInterface
}
JNINativeInterface :: struct {
	reserved0: rawptr
	reserved1: rawptr
	reserved2: rawptr
	reserved3: rawptr
	GetVersion:                    proc "c" (^JNIEnv) -> jint
	FindClass:                     proc "c" (^JNIEnv, cstring) -> jclass
	GetMethodID:                   proc "c" (^JNIEnv, jclass, cstring, cstring) -> ^_jmethodID
	GetStaticMethodID:             proc "c" (^JNIEnv, jclass, cstring, cstring) -> ^_jmethodID
	CallVoidMethodA:               proc "c" (^JNIEnv, jobject, ^_jmethodID, ^jvalue)
	CallVoidMethod:                proc "c" (^JNIEnv, jobject, ^_jmethodID, #c_vararg ..any)
	CallStaticVoidMethodA:         proc "c" (^JNIEnv, jclass, ^_jmethodID, ^jvalue)
	CallStaticVoidMethod:          proc "c" (^JNIEnv, jclass, ^_jmethodID, #c_vararg ..any)
	NewStringUTF:                  proc "c" (^JNIEnv, cstring) -> jstring
	ReleaseStringUTFChars:         proc "c" (^JNIEnv, jstring, cstring)
	ExceptionCheck:                proc "c" (^JNIEnv) -> jboolean
	ExceptionClear:                proc "c" (^JNIEnv)
	RegisterNatives:               proc "c" (^JNIEnv, jclass, ^JNINativeMethod, jint) -> jint
	DeleteGlobalRef:               proc "c" (^JNIEnv, jobject)
	NewGlobalRef:                  proc "c" (^JNIEnv, jobject) -> jobject
}
_jmethodID :: struct {}

foreign import libnativehelper "system:nativehelper" {
	JNI_GetCreatedJavaVMs :: proc(vms: ^^JavaVM, count: jsize, nVms: ^jsize) -> jint
}

JavaVM :: struct {
	using functions: ^JNIInvokeInterface
}
JNIInvokeInterface :: struct {
	GetEnv: proc "c" (^JavaVM, ^rawptr, jint) -> jint
}

jvalue :: struct #raw_union {
	z: jboolean
	b: jbyte
	c: jchar
	s: jshort
	i: jint
	j: jlong
	f: jfloat
	d: jdouble
	l: jobject
}

// ---------------------------------------------------------------------------
// Global ref to the Activity (set from Java), so Odin can call back.
@(private="file")
g_activity: jobject = nil

set_activity_global :: proc(env: ^JNIEnv, activity: jobject) {
	if g_activity != nil {
		env->DeleteGlobalRef(g_activity)
	}
	g_activity = env->NewGlobalRef(activity)
}

// ---------------------------------------------------------------------------
// Native method implementations (match NativeBridge method signatures).

jni_SetAppHome :: proc "c" (env: ^JNIEnv, clazz: jclass, path: jstring) {
	chars := env->GetStringUTFChars(path, nil)
	defer env->ReleaseStringUTFChars(path, chars)
	engine.set_app_home(string(chars))
}

jni_HandleAction :: proc "c" (env: ^JNIEnv, clazz: jclass,
	action: jint, requestJson: jstring, outPath: jstring) -> jint {

	reqChars := env->GetStringUTFChars(requestJson, nil)
	defer env->ReleaseStringUTFChars(requestJson, reqChars)
	outChars := env->GetStringUTFChars(outPath, nil)
	defer env->ReleaseStringUTFChars(outPath, outPath)

	return engine.handle_action(action, string(reqChars), string(outChars))
}

// Odin asks Java to update the UI title.
jni_UiSetTitleText :: proc "c" (env: ^JNIEnv, clazz: jclass, text: jstring) {
	chars := env->GetStringUTFChars(text, nil)
	defer env->ReleaseStringUTFChars(text, chars)

	if g_activity == nil { return }

	cls := env->GetObjectClass(g_activity)
	defer env->DeleteLocalRef(cls)
	mid := env->GetMethodID(cls, "setTitleText", "(Ljava/lang/String;)V")
	if mid == nil {
		env->ExceptionClear()
		return
	}
	jtext := env->NewStringUTF(chars)
	env->CallVoidMethodA(g_activity, mid, [1]jvalue{{l = jtext}})
	env->DeleteLocalRef(jtext)
	if env->ExceptionCheck() != JNI_FALSE {
		env->ExceptionClear()
	}
}

// ---------------------------------------------------------------------------
// JNI_OnLoad: the JVM entry point. Register NativeBridge's native methods.

export
JNI_OnLoad :: proc "c" (vm: ^JavaVM, reserved: rawptr) -> jint {
	var env: ^JNIEnv
	if vm->GetEnv((^rawptr)(&env), JNI_VERSION_1_6) != JNI_OK {
		return JNI_ERR
	}

	cls := env->FindClass("com/oji/hello/NativeBridge")
	if cls == nil { return JNI_ERR }

	methods := [
		JNINativeMethod{ name = "setAppHome", signature = "(Ljava/lang/String;)V", fnPtr = cast(rawptr) jni_SetAppHome },
		JNINativeMethod{ name = "handleAction", signature = "(ILjava/lang/String;Ljava/lang/String;)I", fnPtr = cast(rawptr) jni_HandleAction },
		JNINativeMethod{ name = "uiSetTitleText", signature = "(Ljava/lang/String;)V", fnPtr = cast(rawptr) jni_UiSetTitleText },
	]

	if env->RegisterNatives(cls, &methods[0], i32(len(methods))) != JNI_OK {
		return JNI_ERR
	}
	return JNI_VERSION_1_6
}
```

## D) Java → Odin call flow (retry button)

1. User taps `retry` → `HomeActivity.onRetry()` (user-owned stub) → `NativeBridge.handleAction(ACTION_RETRY, "{}", outPath)`.
2. `JNI_OnLoad`-registered `jni_HandleAction` runs in Odin → `engine.handle_action(...)` writes `{...}` to `outPath`.
3. Java reads `outPath`, gets result.

## E) Odin → Java callback flow (set title)

1. Odin code calls `jni_UiSetTitleText(jstring)`.
2. It looks up the global `g_activity` (set when Java first called `setAppHome` — extend `setAppHome` to also pass the Activity if you want the callback; simpler: call `setActivityGlobal` from Java's `onCreate` via another registered native).
3. Finds `setTitleText(String)` on `com.oji.hello.HomeActivity`, invokes it through `CallVoidMethodA`.

## F) Verify on device

```powershell
adb install build/apk/hello.apk
adb shell am start -n com.oji.hello/com.oji.hello.HomeActivity
adb logcat -s OdinEngine com.oji.hello
```

Tap Retry; logcat should show the Odin `handle_action` result and the title text should change via the callback.
