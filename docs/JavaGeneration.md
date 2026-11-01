# Java Generation

What the generator emits and why is in
[04-codegen-and-templates.md](04-codegen-and-templates.md). The parts that matter
for Java specifically:

- `<Screen>ScreenBase.java` is **generated**: `setContentView(R.layout.<screen>)`,
  one typed `protected` field per widget id, one `findViewById` per field, click
  listeners wired to `protected abstract void on<Name>()`, and
  `setState(String)` when the screen declares states.
- `<Screen>Activity.java` is **user-owned**: created only when missing,
  `extends <Screen>ScreenBase`, empty handler bodies with TODO comments.
- Import lines are limited to the types actually used and sorted like Android
  Studio would.
- Every id becomes a Java field, which is why the validator rejects resource ids
  that are Java keywords (V009).

Reference output: `docs/golden/hello/java/com/oji/hello/`.
