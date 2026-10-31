# Decisions Log

## 2026-10-31 (Initial Seed)
1. **Round-trip editing:** Goal is round-trip, but **v1 is generate-only**.
2. **Generated vs User Split:** `ScreenBase.java` (generated/overwritten) vs `Activity.java` (user-owned/generated-if-missing).
3. **C++ Layering:** `core` <- `parser` <- `validator` <- `scaffolder`. No UI dependencies in lower layers.
4. **Template Renderer:** Own `{{path}}` renderer first; `inja` is an open decision for later.
5. **JNI Backends:** C ABI boundary for JNI backends; Odin/Zig flavors are optional templates later.
6. **Java/XML/Activities:** v1 limited to Java + XML, Activities only, LinearLayout-style `column`/`row` containers (ConstraintLayout is later).

## Open Decisions
- Whether to adopt `inja` template engine later (currently own renderer).
