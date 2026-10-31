# Validator Rules

## Principle
Every crash found on a real phone becomes a new rule.

## Rules (Phase 1)

| Id | Rule | Severity | JSON Path Example | Message |
|---|---|---|---|---|
| V001 | ids are unique within a screen | Error | `$.screens[0].layout.children[1].id` | duplicate id "retry" |
| V002 | every `onClick` name is a valid Java identifier and unique per screen | Error | `$.screens[0].layout.children[1].onClick` | invalid identifier or duplicate onClick "retry" |
| V003 | widget `type` is in the allowed set | Error | `$.screens[0].layout.children[0].type` | unknown widget type "foo" |
| V004 | package name is a valid Java package; app name non-empty | Error | `$.app.package` | invalid package name |
| V005 | screen ids are unique and valid as file/class names | Error | `$.screens[0].id` | invalid screen id or duplicate |
| V006 | `states` only contains known state names; non-empty if present | Error | `$.screens[0].states` | invalid state or empty states |
| V007 | `minSdk` is within a supported range (21 to 35) | Error | `$.app.minSdk` | minSdk out of range |
| V008 | ids are valid Android resource names (lowercase letters, digits, underscores; not starting with a digit) | Error | `$.screens[0].layout.children[0].id` | invalid Android resource id |
