package codegen

import "src:parser"
import "core:fmt"
import "core:strconv"
import "core:strings"

// Emits a minimal Gradle Android app shell under gradle-project/.
// Kept in a subfolder so Phase-1 golden files at the generate root stay stable.

append_gradle_project :: proc(files: ^[dynamic]GeneratedFile, model: parser.AppModel) {
    pkg := model.app.pckg
    name := model.app.name
    if name == "" do name = "App"
    min_sdk := model.app.min_sdk
    if min_sdk < 21 do min_sdk = 21
    min_s := fmt.tprintf("%d", min_sdk)

    settings := strings.concatenate({
        "pluginManagement {\n",
        "    repositories {\n",
        "        google()\n",
        "        mavenCentral()\n",
        "        gradlePluginPortal()\n",
        "    }\n",
        "}\n",
        "dependencyResolutionManagement {\n",
        "    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)\n",
        "    repositories {\n",
        "        google()\n",
        "        mavenCentral()\n",
        "    }\n",
        "}\n\n",
        "rootProject.name = \"", name, "\"\n",
        "include \":app\"\n",
    })
    append(files, GeneratedFile{
        path    = "gradle-project/settings.gradle",
        content = settings,
    })

    root_build := strings.concatenate({
        "// asstudio:generated – root build.gradle\n",
        "plugins {\n",
        "    id 'com.android.application' version '8.2.2' apply false\n",
        "}\n",
    })
    append(files, GeneratedFile{
        path    = "gradle-project/build.gradle",
        content = root_build,
    })

    props := strings.concatenate({
        "org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8\n",
        "android.useAndroidX=true\n",
        "android.nonTransitiveRClass=true\n",
    })
    append(files, GeneratedFile{
        path    = "gradle-project/gradle.properties",
        content = props,
    })

    app_build := strings.concatenate({
        "plugins {\n",
        "    id 'com.android.application'\n",
        "}\n\n",
        "android {\n",
        "    namespace '", pkg, "'\n",
        "    compileSdk 34\n\n",
        "    defaultConfig {\n",
        "        applicationId '", pkg, "'\n",
        "        minSdk ", min_s, "\n",
        "        targetSdk 34\n",
        "        versionCode 1\n",
        "        versionName \"1.0\"\n\n",
        "        ndk {\n",
        "            abiFilters 'arm64-v8a'\n",
        "        }\n",
        "    }\n\n",
        "    buildTypes {\n",
        "        release {\n",
        "            minifyEnabled false\n",
        "        }\n",
        "        debug {\n",
        "            jniDebuggable true\n",
        "        }\n",
        "    }\n\n",
        "    compileOptions {\n",
        "        sourceCompatibility JavaVersion.VERSION_17\n",
        "        targetCompatibility JavaVersion.VERSION_17\n",
        "    }\n\n",
        "    sourceSets {\n",
        "        main {\n",
        "            java.srcDirs = ['src/main/java']\n",
        "            res.srcDirs = ['src/main/res']\n",
        "            manifest.srcFile 'src/main/AndroidManifest.xml'\n",
        "            jniLibs.srcDirs = ['src/main/jniLibs']\n",
        "        }\n",
        "    }\n",
        "}\n\n",
        "dependencies {\n",
        "    implementation 'androidx.appcompat:appcompat:1.6.1'\n",
        "    implementation 'com.google.android.material:material:1.11.0'\n",
        "    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'\n",
        "}\n",
    })
    append(files, GeneratedFile{
        path    = "gradle-project/app/build.gradle",
        content = app_build,
    })

    readme := strings.concatenate({
        "# Gradle project shell (asstudio)\n\n",
        "Copy the Phase-1 generated tree into Android layout:\n\n",
        "```\n",
        "AndroidManifest.xml  -> app/src/main/AndroidManifest.xml\n",
        "res/                 -> app/src/main/res/\n",
        "java/                -> app/src/main/java/\n",
        "native .so           -> app/src/main/jniLibs/arm64-v8a/\n",
        "```\n\n",
        "Then from `gradle-project/`:\n\n",
        "```\n",
        "./gradlew :app:assembleDebug\n",
        "```\n",
    })
    append(files, GeneratedFile{
        path    = "gradle-project/README.md",
        content = readme,
    })

    _ = strconv.parse_int // silence if unused on some Odin versions
}
