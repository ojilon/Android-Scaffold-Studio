# Validator Rules

## Principle

Every crash found on a real phone becomes a new rule. The rule gets a stable id,
a JSON path that points at the model element to fix, and a human message. The
model is the thing that gets fixed; generated files are never hand-edited.

`validator::validate(const parser::AppModel&)` returns
`core::Result<void, std::vector<RuleViolation>>`: success means valid, failure
carries every violation found, in document order. Each
`RuleViolation` has `id`, `path`, `message` and `severity` (`"Error"` today;
`"Warning"` is reserved so the UI can show advisory rules later without
changing the shape).

Because the validator walks the loaded model, paths are reconstructed from the
model's indices: `$.screens[0].layout.children[1].id`.

## Rules (Phase 1)

| Id | Rule | Severity | JSON path example | Message example |
|---|---|---|---|---|
| V001 | ids are unique within a screen, and never shadow the generated state views | Error | `$.screens[0].layout.children[1].id` | `duplicate id "retry"` |
| V002 | every `onClick` is a valid Java identifier and unique per screen after capitalisation | Error | `$.screens[0].layout.children[1].onClick` | `duplicate onClick "Retry" (handler onRetry already declared at ...)` |
| V003 | widget `type` is in the allowed set; `layout.type` is `column` or `row` | Error | `$.screens[0].layout.children[0].type` | `unknown widget type "foo"` |
| V004 | package name is a valid Java package; app name non-empty | Error | `$.app.package` | `invalid Java package name "com.9oji.hello"` |
| V005 | screen ids are unique and valid as file/class names | Error | `$.screens[1].id` | `duplicate screen id "home"` |
| V006 | `states` only contains known names, is non-empty if declared, and includes `content` | Error | `$.screens[0].states[1]` | `unknown state "offline"` |
| V007 | `minSdk` is within the supported range (21 to 35) | Error | `$.app.minSdk` | `minSdk 19 is outside the supported range 21-35` |
| V008 | ids are valid Android resource names (lowercase letters, digits, underscores; not starting with a digit) | Error | `$.screens[0].layout.children[0].id` | `invalid Android resource id "Title"` |
| V009 | ids must not be Java keywords and must not start with the generated state-view prefix | Error | `$.screens[0].layout.children[0].id` | `id "class" is a Java keyword and cannot be a generated field name` |

### Why the two extra rules exist

- **V009** (added 2026-11-01): every widget id becomes a `protected` field in
  `<Screen>ScreenBase.java`. `android:id="@+id/class"` is a legal Android
  resource, but `protected TextView class;` does not compile. Catching it in the
  model is cheaper than debugging a Java error.
- **V006** also requires `content`, because the screen's own layout is displayed
  inside the `content` state view. Declaring `states` without `content` would
  silently drop the developer's widgets from the generated layout.

## Sample Coverage

`samples/invalid/` holds one file per rule family; `tests/test_validator.cpp`
loads each one and asserts the rule id and JSON path:

```
v001_duplicate_id.json              v004_invalid_package.json
v001_nested_duplicate_id.json       v005_duplicate_screen_id.json
v001_reserved_state_id.json         v006_unknown_state.json
v002_invalid_onclick.json           v006_missing_content_state.json
v002_duplicate_onclick.json         v007_min_sdk_out_of_range.json
v003_invalid_type.json              v008_invalid_resource_id.json
                                    v009_java_keyword_id.json
```

## CLI Output

```
$ AndroidScaffoldStudio validate samples/invalid/v001_duplicate_id.json
[V001] $.screens[0].layout.children[1].id: duplicate id "title"
$ echo $?
1
```

`validate` exits 0 when the model is valid, 1 when any rule fails. `generate`
runs the same validation first and refuses to write anything on failure.
