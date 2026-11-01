# Parser / XML

| File | Purpose |
|---|---|
| `Xml.cpp` | `parse_xml` / `write_xml`. Phase 1: stubs that return a `core::Result` error, because Phase 1 goes model -> files only. |
| `XmlAttribute.h/.cpp` | `is_android_attribute`, `is_tools_attribute`, `is_app_attribute` plus the `Attribute` value type for the future importer. |

There are no `XmlReader`/`XmlWriter` classes: the reader and the writer carry no
state between calls, so they are free functions. pugixml is vendored but not
compiled until the Phase 4 importer needs it.

Docs: `docs/08-roadmap.md` (Phase 4), `docs/01-architecture.md`.
