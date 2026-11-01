# Project Generation

## Phase 1 (implemented)

```
parser::AppModel + codegen::TemplateSet
        |
        v
codegen::generate()      -> GenerationOutput { GeneratedFile { relative_path, content, user_owned } }
        |
        v
scaffolder::write_output() -> WriteReport { written, skipped }
        |
        v
<outdir>/AndroidManifest.xml
<outdir>/res/layout/<screen>.xml
<outdir>/java/<package path>/<Screen>ScreenBase.java
<outdir>/java/<package path>/<Screen>Activity.java   (only if missing)
```

`write_output` creates parent directories through `core::fs::write_text_file`,
rewrites generated files every time, and skips user-owned files that already
exist, listing them in `WriteReport::skipped` so the CLI can report
`kept <path> (user-owned, already exists)`.

## Phase 2 (planned)

Full Gradle project skeleton: `settings.gradle`, `build.gradle`,
`gradle.properties`, `res/values/{strings,colors,themes}.xml`, `gradlew` wrapper
copy, plus directories (`src/main/java`, `src/main/res/...`). They will use the
same write path, and the same generated/user-owned distinction (`gradle.properties`
becomes user-owned the moment it exists).

Order of work: directories -> Gradle files -> resources -> manifest -> layouts ->
Java. Each stage fails loudly and leaves nothing half-written for that stage.
