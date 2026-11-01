# XML Parser

## Purpose

The XML parser converts Android XML layout files into the application's
internal widget tree.

The parser never manipulates the UI directly.

Every subsystem operates on the Widget Tree instead of raw XML.

---

## Data Flow

Android XML

↓

pugixml

↓

Widget Tree

↓

Designer

↓

Code Generator

↓

XML Writer

---

## Public API

```cpp
core::Result<Layout> parse_xml(const core::fs::Path& xml_file);
core::VoidResult      write_xml(const Layout& layout, const core::fs::Path& output_file);
```

The parser owns XML parsing.

No other module should call pugixml directly.

---

## Status

Phase 1 does not read XML: both functions return a `core::Result` error. The
generator writes XML itself, and the importer (Phase 4) needs a shape that
preserves attributes it does not understand, which is a different design from
the generator's model.

## Widget Tree

Every XML element becomes one Widget.

Example

```xml
<LinearLayout>

    <Button/>

</LinearLayout>
```

becomes

```
LinearLayout

└── Button
```

Each widget stores

- type
- id
- properties
- children

---

## Property Storage

Every XML attribute is stored exactly as it appears.

Example

```xml
android:text="Hello"
```

becomes

```
properties

android:text

↓

Hello
```

Interpretation of those properties belongs to the Designer,
not the parser.

---

## Recursive Algorithm

Reading

```
Node

↓

Read Attributes

↓

Read Children

↓

Return Widget
```

Writing

```
Widget

↓

Create XML Node

↓

Write Attributes

↓

Write Children
```

---

## Responsibilities

- Read XML
- Write XML
- Preserve hierarchy
- Preserve attributes
- Report parsing errors

---

## Non Responsibilities

The parser does not

- render widgets
- edit widgets
- validate Android layouts
- optimize XML
- generate Java

---

## Future Work

- Namespace preservation
- Comment preservation
- Formatting preservation
- XML validation
- Better diagnostics
- Unknown node preservation
- Round-trip editing