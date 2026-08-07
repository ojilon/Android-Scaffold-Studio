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
result::Result<Layout>
parse_xml(...);

result::VoidResult
write_xml(...);
```

The parser owns XML parsing.

No other module should call pugixml directly.

---

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