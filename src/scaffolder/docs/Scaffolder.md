# Scaffolder

## Pipeline

Project Model

↓

Directory Builder

↓

Template Engine

↓

Project Writer

↓

Android Project

---

## Generated Files

AndroidManifest.xml

build.gradle

settings.gradle

gradle.properties

local.properties

proguard-rules.pro

res/

src/

assets/

libs/

---

## Design Goals

The scaffolder should never hardcode file contents.

Every generated file comes from a reusable template.

This allows users to customize templates without changing C++ code.

Future versions may support multiple template packs:

- Android App
- Android Library
- Jetpack Compose
- JNI
- Multi-module

## Template System

The scaffolder never generates Android files directly.

Instead it follows this pipeline.

```
Template

↓

Load

↓

Replace Variables

↓

Write File
```

This allows templates to be edited without recompiling the application.

Future template packs may include:

- Android Application
- Android Library
- Jetpack Compose
- JNI Project
- Multi-module Project
- Kotlin-only Project
- C++ Native Project