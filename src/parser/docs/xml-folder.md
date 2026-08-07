## Internal Architecture

The XML parser is divided into four layers.

```
Xml.cpp
```

Public API only.

```
XmlReader
```

Responsible for converting XML into the Widget Tree.

```
XmlWriter
```

Responsible for converting the Widget Tree back into XML.

```
XmlAttribute
```

Contains reusable attribute helper functions (namespace checks, classification, future validation).

This separation keeps each file focused on one responsibility and avoids one large implementation file.