# Template Engine

The renderer lives in `codegen` (`include/codegen/Template.h`,
`codegen/Template.cpp`); the scaffolder only loads template text from disk.

## Syntax

```text
package {{app.package}};

{{screen.modifiers}} class {{screen.className}} extends AppCompatActivity {
{{screen.fields}}
```

- `{{dotted.path}}` reads a value from a `nlohmann::json` object.
- Whitespace inside the braces is ignored: `{{ app.name }}`.
- No loops, no conditionals, no includes. Fragments (fields, listeners, state
  views) are rendered in C++ and passed in as strings.
- Substituted text is never rescanned, so a value containing `{{` cannot loop.
- An unknown placeholder or a non-string value is an error naming the template and
  the placeholder. Silent leftovers would ship broken Java.

## Error format

```
ScreenBase.java.in: unknown template variable {{screen.clazzName}}
```

## Future

`inja` remains an open decision (docs/DECISIONS.md). It would only be worth it
once templates need real logic; today every construct is a substitution.
