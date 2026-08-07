# Template Engine

## Purpose

Transforms text templates into finished files.

---

## Example

Template

```text
package ${PACKAGE}

class ${CLASS}
```

Variables

```
PACKAGE

↓

com.example.app

CLASS

↓

MainActivity
```

Output

```java
package com.example.app;

class MainActivity
```

---

## Supported

Current

- Variable replacement

Future

- if
- else
- loops
- include
- nested templates

---

## Design Goal

The template engine is intentionally generic.

It should be capable of generating

- XML
- Java
- Kotlin
- C++
- Gradle
- Markdown
- JSON
- YAML
- GitHub Actions
- CMake
- Any text-based format