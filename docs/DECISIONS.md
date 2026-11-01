# Decisions Log

Dated, newest last. Each entry says what was decided and why; open questions stay
at the bottom.

## 2026-10-31 (initial seed)

1. **Round-trip editing:** the goal is round-trip, but **v1 is generate-only**.
2. **Generated vs user-owned split:** `ScreenBase.java` (generated/overwritten)
   vs `Activity.java` (user-owned, generated only when missing).
3. **C++ layering:** `core` <- `parser` <- `validator` <- `codegen`/`scaffolder`.
   No UI dependencies in lower layers.
4. **Template renderer:** own `{{path}}` renderer first; `inja` is an open
   decision for later.
5. **JNI backends:** C ABI boundary for JNI backends; Odin/Zig flavours are
   optional templates later.
6. **Java/XML/Activities:** v1 is limited to Java + XML, Activities only,
   LinearLayout-style `column`/`row` containers (ConstraintLayout is later).

## 2026-11-01 (post-device-try cleanup)

7. **No classes in C++; structs plus free functions.** Owner preference, now a
   hard rule in AGENTS.md. Consequences:
   - the parser's `XmlReader`/`XmlWriter` classes are gone; `parse_xml` /
     `write_xml` are free functions, and the duplicate `parser::Widget` from
     `WidgetTree.h` (an ODR clash with `AppModel.h`) is deleted in favour of the
     model's own `Widget` + `TreeUtils.h` helpers;
   - `ParseContext` keeps its fields but `add_error`/`has_errors` are free
     functions;
   - scaffolder's `ProjectValidator`, `DirectoryBuilder`, `ResourceGenerator`,
     `ProjectWriter`, `Scaffolder` and `TemplateEngine` classes are replaced by
     two small headers (`Templates.h`, `ProjectWriter.h`) with free functions;
   - codegen's `Scaffolder.h` (which contained a full copy of the generator in a
     header) is replaced by `Generation.h` + `Template.h`.
8. **`std::expected` is required; no fallback branch.** The owner's GCC 15.2 and
   the sandbox's GCC 12.2 both ship it. Nothing is gained by carrying a C++20
   `Result<T,E>` that no machine builds, so the fallback is documented in
   docs/06-stack-and-libs.md and not implemented.
9. **No `<format>` anywhere.** GCC 12 does not provide it, and the logger is a
   `std::ostringstream`. Keeps the tree buildable on older toolchains.
10. **The validator takes the loaded `AppModel`, not the raw JSON text.** One
    parse, one source of truth for `validate` and `generate`, and
    `nlohmann/json` stays inside `parser`. JSON paths are rebuilt from model
    indices, so CLI output still points at the element to fix.
11. **Output paths follow real Android conventions.** `res/layout/<screen>.xml`
    with `R.layout.<screen>`, Java under `java/<package path>/`, and a golden
    folder that mirrors the output tree (`docs/golden/hello/res/...`). The
    previous `res_layout_home.xml` naming and flat golden files were deviations
    from the plan's "follow real Android conventions".
12. **Templates are files, loaded at run time.** `scaffolder` reads them from
    `templates/`; the directory is recorded at configure time
    (`scaffolder/Config.h`) and can be overridden with `--templates`. Keeping
    them out of the binary is what makes the Phase 2 template library possible
    without a rebuild.
13. **`codegen` stays pure and `scaffolder` writes.** `generate()` returns text
    and flags; only `scaffolder::write_output()` knows about directories and the
    overwrite policy. This is what keeps the golden test a string comparison.
14. **`states` must include `content` (V006).** The screen's own layout is
    rendered inside the content state view; declaring states without `content`
    would silently drop the developer's widgets.
15. **Generated state-view ids are reserved (V001).** `<screen>_state_*` belongs
    to the generator. A model widget reusing one would produce duplicate ids, and
    `findViewById` would return the wrong view at runtime.
16. **V009: ids must not be Java keywords.** Every id becomes a `protected` field
    in the generated base class; `android:id="@+id/class"` is valid Android but
    `protected TextView class;` is not Java. Cheap to catch in the model.
17. **Manifest: first screen is the launcher, every activity declares
    `android:exported`.** Missing `exported` is an install failure from targetSdk
    31; we emit `true` for the launcher and `false` for the rest.
18. **Vendored headers are real files at pinned versions.** They were 0-byte
    placeholders in the tree, which made every test link impossible. Versions and
    licences are recorded in `third_party/README.md`.
19. **CMake floor lowered to 3.25** (from 4.0) with the `CMP0171` guard, so both
    CMake 3.x and 4.4 configure the same tree. `CMakePresets.json` was empty (0
    bytes); it now has `default`, `release` and `asan` presets, and `ASSTUDIO_WERROR`
    / `ASSTUDIO_SANITIZE` are the documented knobs.
20. **Builder-style names are snake_case.** Functions, variables, files:
    `load_model`, `write_output`, `render`, `validate`. Types stay `PascalCase`
    and model fields keep their JSON spelling (`minSdk`, `onClick`).

## Open Decisions

- **`inja` vs our renderer:** unchanged. Our renderer is ~90 lines and handles
  the whole Phase 1/2 need; adopt `inja` only if template logic (loops,
  conditionals) becomes common enough to justify the dependency.
- **`yyjson` instead of nlohmann:** only if compile time on the Celeron becomes
  painful; currently two translation units include it.
- **`android:exported` for non-launcher screens:** always `false` today; when
  deep links arrive, that needs a model field.
- **Manifest `package` attribute vs AGP namespace:** `package` is emitted for
  compatibility. Phase 2's Gradle generation should move it to `namespace` and
  drop the attribute.
