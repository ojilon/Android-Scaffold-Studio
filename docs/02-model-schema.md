# App Model v1 Schema

`model.json` is the single source of truth. Generated XML, Java and the manifest
are all derived from it; nothing is authored in the output.

## JSON Structure

```json
{
  "schemaVersion": 1,
  "app": { "name": "Hello", "package": "com.oji.hello", "minSdk": 21 },
  "screens": [{
    "id": "home",
    "states": ["loading", "content", "error"],
    "layout": { "type": "column", "children": [
      { "type": "text",   "id": "title", "text": "Hello" },
      { "type": "button", "id": "retry", "text": "Retry", "onClick": "retry" }
    ]}
  }]
}
```

`schemaVersion` and `screens` are required. Unknown keys are ignored on purpose,
so an older tool can still load a newer model.

## App

| Field | Type | Rules |
|---|---|---|
| `name` | string | non-empty (V004) |
| `package` | string | valid dotted Java package, no keyword segments (V004) |
| `minSdk` | int, default 21 | 21..35 (V007) |

## Screens

| Field | Type | Notes |
|---|---|---|
| `id` | string | `[a-z][a-z0-9_]*`, unique; also the layout file name and the class-name stem (`home` -> `HomeScreenBase`, `HomeActivity`) |
| `states` | array, optional | subset of `loading`, `content`, `error`; if present it must be non-empty and must include `content` (V006) |
| `layout` | object | `type` is `column` or `row` |
| `backend` | string, reserved | `"jni_json_bridge"` is parsed and ignored in v1 |

Every screen produces its own files; the **first screen in the array is the
launcher activity**. No reordering logic exists, so the order is meaningful.

## Widgets

`layout.children` and nested `children` hold widgets. Allowed `type` values in
v1 (anything else is V003):

- `column` -> `<LinearLayout android:orientation="vertical">`
- `row` -> `<LinearLayout android:orientation="horizontal">`
- `text` -> `<TextView>`
- `button` -> `<Button>`
- `input` -> `<EditText>` (`text` becomes `android:hint`)
- `image` -> `<ImageView>` (`text` becomes `android:contentDescription`)
- `spacer` -> `<Space>`

Common attributes:

| Field | Meaning |
|---|---|
| `id` | Android resource id `[a-z][a-z0-9_]*`, unique per screen (V001/V008), not a Java keyword (V009) |
| `width` / `height` | `match_parent`, `wrap_content`, or a literal such as `16dp`. Empty means: containers `match_parent` wide / `wrap_content` tall, everything else `wrap_content` |
| `margin` | `android:layout_margin` (`16dp`, `8dp`, ...) |
| `padding` | `android:padding` |
| `text` | see the per-type mapping above |
| `onClick` | handler name without the `on` prefix; `retry` -> `protected abstract void onRetry()`. Valid Java identifier, unique per screen after capitalisation (V002) |

Nesting is supported: a `row` inside a `column` inside a `column`, etc.

## Screen States Convention

A screen that declares `states` gets a `FrameLayout` root holding one wrapper
`LinearLayout` per declared state:

| State | Generated view |
|---|---|
| `loading` | centred `ProgressBar` |
| `content` | the screen's own layout (its widgets) |
| `error` | message `TextView` + `Retry` `Button` |

The generated base class then contains:

```java
public void setState(String state) {
    homeStateLoading.setVisibility("loading".equals(state) ? View.VISIBLE : View.GONE);
    ...
}
```

`setState` always shows exactly one state, and `onCreate` calls it with the first
declared state so a screen never opens in an undeclared state.

Naming is reserved and generated: the views are `<screen>_state_<state>`,
`<screen>_state_error_message` and `<screen>_state_error_retry`; the Java fields
are their camelCase equivalents (`homeStateError`). A model widget using one of
those ids is rejected by V001. The error retry button shares the screen's
`onRetry()` handler, so the generated retry and the developer's own retry button
can never drift apart.

A screen without `states` gets a plain `LinearLayout` root and no `setState`
method at all - no dead code.
