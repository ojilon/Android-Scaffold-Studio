# Golden parity notes (odin-rewrite)

Target: `./build/asstudio generate samples/hello.json -o /tmp/gen`
matches `docs/golden/hello/` (normalize CRLF).

## Field rename

Odin reserves `package`. The model field is **`app.pckg`** in code;
JSON still uses `"package"` (loader maps it).

## Handler method names (important)

Golden expects **PascalCase after `on`**:

| model onClick | Java method |
|---------------|-------------|
| `retry`       | `onRetry`   |

In `generate.odin`, prefer:

```odin
method := strings.concatenate({"on", capitalize(w.on_click)})
```

not a raw concat of `"on"` + `w.on_click` (that yields `onretry`).

## Imports

CLI build uses collections:

```
odin build src/app -out:build/asstudio -collection:src=src
```

So packages import as `src:parser`, `src:validator`, `src:codegen`.
Relative `../parser` only works in some layouts; prefer collections for the CLI.

## Checklist after generate

```
AndroidManifest.xml
res/layout/home.xml
java/com/oji/hello/HomeScreenBase.java
java/com/oji/hello/HomeActivity.java
```

Diff should be empty aside from optional trailing newline.
