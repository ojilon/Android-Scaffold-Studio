# Widget Tree

## Purpose

The Widget Tree is the central data model of the application.

Everything operates on this structure instead of directly editing XML.

---

## Data Flow

Android XML

↓

Widget Tree

↓

Designer

↓

Property Editor

↓

Code Generator

↓

Android XML

---

## Widget

Each node stores

- type
- id
- properties
- children

Example

```xml
<LinearLayout>

    <Button/>

    <TextView/>

</LinearLayout>
```

becomes

```
LinearLayout

├── Button

└── TextView
```

---

## Tree Utilities

The helper functions provide

visit()

Recursive traversal.

find_by_id()

Find a widget by Android id.

find_by_type()

Collect widgets of one type.

count_widgets()

Returns the total number of widgets.

---

## Design Rules

Widgets own their children.

No widget stores a parent pointer.

Traversal is recursive.

Widget Tree never depends on XML.

Widget Tree never depends on Java.

Widget Tree never depends on the Designer.

Every subsystem depends on the Widget Tree.

It is the shared language of the application.