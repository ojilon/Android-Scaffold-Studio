# Widget Tree (parser::Widget)

## Purpose

The widget tree is the central data model of the application: a
`parser::Widget` with nested `children`, defined in
`include/parser/AppModel.h`.

Everything operates on this structure instead of directly editing XML. There is
no `WidgetTree` type of its own any more - the model's `Widget` *is* the tree,
which removes the duplicate definition that used to clash between
`WidgetTree.h` and `AppModel.h`.

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

## Tree Utilities (`include/parser/TreeUtils.h`)

The free functions provide

visit()

Recursive traversal.

find_by_id()

Find a widget by Android id.

find_by_type()

Collect widgets of one type.

count_widgets()

Returns the total number of widgets.

all_ids()

Every non-empty id, in document order (used by tests and the future UI).

---

## Design Rules

Widgets own their children.

No widget stores a parent pointer.

Traversal is recursive.

Widget Tree never depends on XML.

Widget Tree never depends on Java.

Widget Tree never depends on the Designer.

Every subsystem depends on this model.

It is the shared language of the application. Because it is a plain struct, it is
cheap to copy, compare and construct in tests; all behaviour lives in free
functions.