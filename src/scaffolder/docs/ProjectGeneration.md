# Project Generation

## Overview

The scaffolder converts the internal Project model into a complete Android project on disk.

```
Project Model

↓

Validation

↓

Directory Creation

↓

Template Expansion

↓

Resource Generation

↓

File Writing

↓

Android Project
```

---

## Generation Order

1. Validate the project.
2. Create directories.
3. Load templates.
4. Expand template variables.
5. Generate resources.
6. Write all files.
7. Verify output.

---

## Resource Generation

The ResourceGenerator is responsible for creating:

- layout/
- values/
- drawable/
- mipmap/
- menu/
- navigation/
- xml/

Each resource type is generated independently.

---

## Validation

ProjectValidator performs checks before any files are written.

Typical checks include:

- Missing project name
- Invalid Java package
- Unsupported SDK versions
- Duplicate identifiers
- Missing templates

---

## Future Extensions

- Compose project generation
- Android library projects
- Dynamic feature modules
- Native (JNI) projects
- Kotlin templates
- Custom template packs