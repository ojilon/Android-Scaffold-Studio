# App Model v1 Schema

## JSON Structure
```json
{
  "schemaVersion": 1,
  "app": {
    "name": "Hello",
    "package": "com.oji.hello",
    "minSdk": 21
  },
  "screens": [{
    "id": "home",
    "states": ["loading", "content", "error"],
    "layout": {
      "type": "column",
      "children": [
        { "type": "text",   "id": "title", "text": "Hello" },
        { "type": "button", "id": "retry", "text": "Retry", "onClick": "retry" }
      ]
    }
  }]
}
```

## Allowed Widget Types (v1)
- `column`
- `row`
- `text`
- `button`
- `input`
- `image`
- `spacer`

## Common Attributes
- `id`: unique identifier
- `width` / `height`: `match_parent`, `wrap_content`, or `NNdp`
- `margin` / `padding`
- `text`
- `onClick`

## Screen States Convention
A screen with `states` gets a root container holding three state views:
- `loading` = centered ProgressBar
- `content` = the screen layout
- `error` = message + retry
`setState(State)` shows exactly one.

## Reserved Fields
- `"backend": "jni_json_bridge"` on a screen (reserved, not implemented in v1)
