# Codegen Module

## Purpose

Turns an app model plus a template set into the **text** of every generated file.

```cpp
core::Result<GenerationOutput> generate(const parser::AppModel& model,
                                        const TemplateSet& templates);
```

## Rules

- **Pure.** No disk access, no globals, no logging. `generate` is a function of
  its arguments, which is what makes `tests/test_codegen.cpp` a plain string
  comparison against `docs/golden/hello/`.
- **Structs only.** `TemplateSet`, `GeneratedFile`, `GenerationOutput`, plus free
  functions. No classes.
- **Templates own the skeleton.** `templates/xml/layout*.xml.in`,
  `templates/java/*.java.in`, `templates/android/AndroidManifest.xml.in`.
  Placeholders are `{{dotted.path}}`; loops and fragments are rendered in C++ and
  passed in as strings.
- **User-owned files are marked, not written here.** `GeneratedFile::user_owned`
  is `true` for `<Screen>Activity.java`; `scaffolder` decides what to do with it.

## Files

| File | Contents |
|---|---|
| `include/codegen/Template.h`, `Template.cpp` | the `{{dotted.path}}` renderer; unknown variables are errors |
| `include/codegen/Generation.h`, `Generation.cpp` | model -> layout XML, ScreenBase.java, Activity.java, AndroidManifest.xml |

Docs: `docs/04-codegen-and-templates.md`.
