# App Module

The CLI: `src/app/main.cpp` -> executable `AndroidScaffoldStudio`.

```
AndroidScaffoldStudio validate <model.json>
AndroidScaffoldStudio generate <model.json> [-o <outdir>] [--templates <dir>]
AndroidScaffoldStudio --help | --version
```

Exit codes: `0` success/valid, `1` invalid model or failed generation, `2` bad
command line.

The CLI owns argument parsing and exit codes only. It wires `parser` ->
`validator` -> `codegen` -> `scaffolder` and nothing else; `main` is also the one
place where an exception may escape to the runtime.
