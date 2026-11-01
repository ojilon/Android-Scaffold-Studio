# Codegen and Templates

## Division of Labour

```
codegen::generate(model, templates) -> GenerationOutput { files: [{ relative_path, content, user_owned }] }
scaffolder::write_output(output, dir) -> WriteReport { written, skipped }
```

`codegen` is a pure function: no disk access, no globals. That is what makes the
golden test a plain string comparison and what will let the Phase 3 UI show text
before writing it. `scaffolder` owns the directory layout and the
generated/user-owned overwrite policy.

Both are free functions over structs (`TemplateSet`, `GeneratedFile`,
`GenerationOutput`, `WriteReport`); there are no classes in the pipeline.

## Templates

Templates live in `templates/` and are read at run time, never compiled in:

| File | Used for |
|---|---|
| `templates/xml/layout.xml.in` | screens without states (`LinearLayout` root) |
| `templates/xml/layout_states.xml.in` | screens with states (`FrameLayout` root) |
| `templates/java/ScreenBase.java.in` | the generated `<Screen>ScreenBase.java` |
| `templates/java/Activity.java.in` | the user-owned `<Screen>Activity.java` |
| `templates/android/AndroidManifest.xml.in` | the manifest |
| `templates/gradle/build.gradle.in` | Phase 2 placeholder |
| `templates/cpp/jni_json_bridge/` | Phase 2 stub + README (design in docs/05) |

`scaffolder::default_template_directory()` returns the `templates/` path recorded
by CMake at configure time, so the CLI and the tests work from any working
directory. `--templates <dir>` overrides it.

## The Renderer

Phase 1 uses our own renderer (`codegen/Template.h`), not inja:

- `{{dotted.path}}` substitution from a JSON value; whitespace inside braces is
  ignored (`{{ app.package }}`).
- No loops, no conditionals, no includes. Loops and fragments are built in C++
  and passed in as pre-rendered strings - that is why the templates contain
  placeholders such as `{{screen.fields}}` or `{{layout.stateViews}}`.
- Inserted text is never rescanned, so a value containing `{{` cannot loop.
- An unknown placeholder or a non-string value is an **error**, reported with
  the template name. Silent leftovers would end up in generated Java.

Adopting `inja` stays an open decision (docs/DECISIONS.md); it is heavy for the
target hardware and unnecessary while the only constructs are substitutions.

## Generated Artefacts

Per screen:

1. `res/layout/<screen>.xml` - generated, always overwritten.
2. `java/<package path>/<Screen>ScreenBase.java` - generated, always
   overwritten. Contains: `setContentView(R.layout.<screen>)`, one typed field
   and one `findViewById` per widget id, click listeners wired to abstract
   handlers, `protected abstract void on<Name>();` per handler, and
   `setState(String)` plus its initial call when states are declared. Imports
   are limited to types actually used and sorted as Android Studio would.
3. `java/<package path>/<Screen>Activity.java` - user-owned, written only if
   missing. `extends <Screen>ScreenBase` and overrides the handlers with TODO
   bodies.
4. `AndroidManifest.xml` - generated once, one `<activity>` per screen. The
   first screen gets the MAIN/LAUNCHER intent filter and `exported="true"`; all
   others get `exported="false"`. That attribute is required from targetSdk 31
   and forgetting it is a classic install-time failure.

Marker comments: `<!-- asstudio:generated v1 DO NOT EDIT -->` for XML,
`// asstudio:generated v1 DO NOT EDIT` for Java. User-owned files never carry
one.

## Layout XML Conventions

- Model `column`/`row` become `LinearLayout` with the matching orientation.
- Attribute order is fixed (id, size, orientation, margin, padding, text) so the
  output is diff-stable.
- Values are XML-escaped (`&`, `<`, `>`, `"`, `'`), so "Fish & Chips" survives.
- Children are separated by blank lines and the file ends with a newline; the
  generator always writes LF.
- State views are synthesized in C++ (they are not model widgets) and passed to
  the states template as one fragment.

## Golden Test

`docs/golden/hello/` mirrors the output tree:

```
AndroidManifest.xml
res/layout/home.xml
java/com/oji/hello/HomeScreenBase.java
java/com/oji/hello/HomeActivity.java
```

`tests/test_codegen.cpp` generates from `samples/hello.json`, compares every
file byte-for-byte after normalising CRLF, and also checks the `user_owned`
flags, the escaping, the no-states case, and that `write_output` leaves an
existing `HomeActivity.java` untouched.
