# Parser Module

## Purpose

The parser converts files on disk into the application's
internal representation.

No other subsystem should directly parse XML, Java or Gradle.

The parser is the single source of truth for reading Android
projects.

---

## Responsibilities

- Read Android XML layouts.
- Read Java source files.
- Read Gradle build files.
- Convert parsed data into internal structures.
- Preserve as much information as possible.

---

## Non Responsibilities

The parser never

- edits widgets
- generates XML
- generates Java
- renders UI
- creates projects

Those tasks belong to other modules.

---

## Folder Layout

include/parser/

Public API

xml/

Android XML parser

java/

Java parser

gradle/

Gradle parser

---

## Public API

The application should only include

```cpp
#include <parser/Parser.h>
```