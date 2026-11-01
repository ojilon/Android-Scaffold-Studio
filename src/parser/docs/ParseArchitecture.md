# Parser Architecture

## Overview

The parser subsystem converts external project files into the
internal data model used by the application.

Every parser follows the same structure.

```
External File

↓

Parser API

↓

Reader

↓

Widget Tree / Internal Model
```

---

## Public API

Parser.h exposes every parser.

Application code should include only

```cpp
#include <parser/Parser.h>
```

---

## Components

### AppModel (AppModel.h)

The shared document model: `Widget`, `Layout`, `Screen`, `App`, `AppModel`.
Plain structs, no behaviour. There is no separate `WidgetTree` type any more.

---

### TreeUtils

Traversal and searching.

---

### ParseContext

Shared parser configuration.

Contains

- source file
- parser options
- collected diagnostics

---

### ParseError

Standard parser error.

Every parser reports errors using the same structure.

---

### Xml.cpp

`parse_xml` / `write_xml` free functions (Phase 4 stubs). No reader/writer
classes: they would carry no state between calls.

---

### Java Parser

Reads Java sources.

---

### Gradle Parser

Reads Gradle projects.

---

## Future Parsers

The same architecture will later support

- AndroidManifest.xml
- Kotlin
- resources.xml
- colors.xml
- strings.xml
- styles.xml
- navigation.xml

without changing the overall design.

---

## Design Rules

- Parsers never modify projects.
- Parsers never render UI.
- Parsers never generate code.
- Parsers only convert external files into internal models.
- All parsers report diagnostics through ParseContext.