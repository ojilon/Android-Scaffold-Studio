# UI Plan (Documentation Only - Phase 1)

## Future UI Framework
- **Dear ImGui (docking)** with a custom canvas: immediate-mode, light on weak hardware, distinct tool-like look, good for the panels plus a side-by-side code view.

## Preview
- The **preview is our own renderer** drawing the model with approximate Material widgets, not an Android runtime.
- The validator keeps the preview honest.

## Fast Feedback Layers
1. Instant approximate canvas preview
2. "Run on device" loop: generate, `gradlew installDebug`, `adb logcat` piped into the tool, with crashes shown next to the model element that caused them (see concept image 1)

## State Management
The UI never owns state; it edits the model and re-runs validate/generate.
