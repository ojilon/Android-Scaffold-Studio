# Scaffolder

## Purpose

Creates complete Android project structures from the internal
Project model.

The scaffolder owns every generated file on disk.

---

## Responsibilities

- Create folders
- Generate Gradle files
- Generate AndroidManifest.xml
- Generate resources
- Expand templates
- Write files

---

## Non Responsibilities

The scaffolder never

- parses XML
- edits layouts
- renders widgets
- generates Java logic
- generates JNI

Those belong to later modules.