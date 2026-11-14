# Status – odin-rewrite branch

**Updated:** 2026-11-14  
**Branch:** `odin-rewrite`

## Done (Phase 1 foundation)

| Piece | Notes |
|-------|--------|
| Odin model + JSON loader | `app.pckg` (JSON key still `package`) |
| Validator V001–V009 | |
| Codegen XML + Java | states, ScreenBase / Activity split |
| Handler names | `on` + capitalize → `onRetry` |
| CLI | `validate` / `generate` / `--help` |
| Collections | `import "src:parser"` + `-collection:src=src` |
| WayDirect templates | xml / java / jni / odin |
| Gradle templates | `templates/gradle/*` |
| Gradle **emit** | `generate_gradle.odin` → `gradle-project/` |
| Native **emit** | when screen `backend` is `odin_jni` / `jni_json_bridge` |
| Build scripts | `tools/build-cli.sh` (+ your Windows bat) |

## Remaining (ordered)

1. **Confirm golden** – `diff` Phase-1 four files vs `docs/golden/hello/` (ignore `gradle-project/`).
2. **Map flat output → Android sourceSets** – optional `asstudio project` command that copies manifest/java/res into `gradle-project/app/src/main/`.
3. **Real JNI glue** – `JNI_OnLoad` + `Java_*` exports calling `handle_action` (WayDirect-shaped); build `.so` with NDK.
4. **Odin tests** – loader, each rule id, golden snapshot test.
5. **Richer templates** – themes, nav shell, list/detail, charts (slow add).
6. **Desktop designer GUI** – later; CLI is the contract.

## Phase 1 is enough to**

- Author `model.json`
- Validate Android rules
- Generate layout + Java skeleton + optional Gradle shell + optional native stubs
- Hand-fill Activity handlers and grow the Odin engine

## Build / run

```sh
odin build src/app -out:build/asstudio -collection:src=src
./build/asstudio validate samples/hello.json
./build/asstudio generate samples/hello.json -o generated/hello
# golden: compare the four core files only
diff generated/hello/AndroidManifest.xml docs/golden/hello/AndroidManifest.xml
```

To exercise native emit, set on a screen:

```json
"backend": "odin_jni"
```
