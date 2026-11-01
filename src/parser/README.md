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

```
include/parser/     public API (AppModel, AppModelLoader, TreeUtils,
                    ParseContext, ParseError, Xml, Java, Gradle, Parser.h)
AppModelLoader.cpp  model.json -> parser::AppModel (nlohmann/json lives here)
tree/               TreeUtils.cpp: visit / find_by_id / find_by_type /
                    count_widgets / all_ids over parser::Widget
context/            ParseContext.cpp: add_error / has_errors
error/              ParseError.cpp (stub for future diagnostics helpers)
xml/                parse_xml / write_xml stubs + attribute helpers
java/, gradle/      Phase 4 importer stubs
docs/               parser design notes
```

## Design

Structs for data, free functions for behaviour - no classes. `parser::Widget`
from `AppModel.h` is the one widget type in the tree; the old `WidgetTree.h`
duplicate is gone.

The loader reports **structural** problems only (missing file, bad JSON, wrong
type, missing required field) with a JSON path. Android semantics belong to the
validator, so `validate` can report rule ids instead of a parse failure.

## Public API

Application code should include

```cpp
#include <parser/Parser.h>
```
