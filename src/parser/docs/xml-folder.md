## Internal Architecture

The XML folder holds three things:

```
Xml.cpp            public API: parse_xml / write_xml (Phase 4 stubs)
XmlAttribute.h     the Attribute value type
XmlAttribute.cpp   attribute helpers: is_android_attribute, is_tools_attribute,
                   is_app_attribute
```

There is no `XmlReader` / `XmlWriter` class pair: reading and writing XML carry no
state between calls, so they are free functions returning `core::Result`, and the
pugixml document owns its own memory.

The importer (Phase 4) will keep the same two entry points; only their bodies
change. pugixml is vendored in `third_party/pugixml` and is not compiled until
then.
