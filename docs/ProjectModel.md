# Project Model

Two models exist and they are not the same thing:

| Model | Type | What it is |
|---|---|---|
| App model (v1) | `parser::AppModel` | the designed app: app metadata, screens, widgets, states. Documented in [02-model-schema.md](02-model-schema.md) |
| Android project | `core::project::Project` | the physical project Phase 2 writes into: name, package, root directory, modules. Reserved, unused in Phase 1 |

The app model is the single source of truth; the project model only says where
the generated tree lives and which Gradle modules exist.
