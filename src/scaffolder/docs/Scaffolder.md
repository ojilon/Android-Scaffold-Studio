# Scaffolder

## Pipeline

```
templates/  ->  scaffolder::load_templates()  ->  codegen::TemplateSet
model.json  ->  parser + validator            ->  parser::AppModel
                        |
                        v
              codegen::generate(model, templates)   (pure, no disk)
                        |
                        v
              scaffolder::write_output(output, dir) (disk, overwrite policy)
```

## Public surface

```cpp
// include/scaffolder/Templates.h
core::fs::Path default_template_directory();
core::Result<codegen::TemplateSet> load_templates(const core::fs::Path& directory);

// include/scaffolder/ProjectWriter.h
struct WriteReport { std::vector<core::fs::Path> written, skipped; };
core::Result<WriteReport> write_output(const codegen::GenerationOutput&, const core::fs::Path&);
```

No classes: the six class skeletons that used to live here
(`ProjectValidator`, `DirectoryBuilder`, `ResourceGenerator`, `ProjectWriter`,
`Scaffolder`, `TemplateEngine`) were empty stubs whose responsibilities are now
either a free function here or in `codegen`.

## Files

| File | Purpose |
|---|---|
| `include/scaffolder/Templates.h`, `template/TemplateLoader.cpp` | read the template set, with the configure-time default directory |
| `include/scaffolder/ProjectWriter.h`, `writer/ProjectWriter.cpp` | write the tree, skip existing user-owned files |
| `include/scaffolder/Config.h.in` | configured into the build dir: `ASSTUDIO_TEMPLATE_DIR` |
