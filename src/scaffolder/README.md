# Scaffolder Module

Two jobs, both structs + free functions:

1. **Template loading** - `include/scaffolder/Templates.h`:
   `default_template_directory()` and `load_templates(dir)` read
   `templates/xml/*.in`, `templates/java/*.in` and
   `templates/android/AndroidManifest.xml.in` into a
   `codegen::TemplateSet`. The default directory is recorded by CMake
   (`scaffolder/Config.h`); `--templates` overrides it.
2. **Writing the output tree** - `include/scaffolder/ProjectWriter.h`:
   `write_output(output, dir)` writes every `codegen::GeneratedFile`, creating
   directories as needed, and returns a `WriteReport` of what was `written` and
   what was `skipped` because it is user-owned and already exists.

The scaffolder never decides what a file contains (that is `codegen`), and
`codegen` never touches the disk (that is this module).

Phase 2 will add the full Gradle project skeleton here (directories, Gradle
files, `res/values`, themes) using the same write path.
