# Vision for Android Scaffold Studio

## The Problem
Hand-writing Android XML and Java is painful. Specific pains:

1. Apps build successfully and then **crash on the phone** because XML and Java disagree (missing ids, wrong view types, unregistered activities, bad themes).
2. The developer cannot visualize runtime while coding; even AI assistants write on assumptions. The feedback loop between coding and previewing is slow.
3. Many patterns repeat: JNI communication, screen states, buttons, sidebars, layouts. These should be templates/codegen, standardized.
4. Android must be targeted by **rules**, even though the design happens in a PC app.

## The Goal
A **standalone native desktop tool** where the developer designs UI/UX (eventually by drag and drop) on top of a single source of truth (an **app model**), the tool **validates it against Android rules**, **generates** correct XML/Java/manifest/Gradle (and JNI plumbing), and eventually gives fast feedback (instant approximate preview, then run-on-device with crashes mapped to model elements).

## Later Ambition
**First-class round-trip editing** (import existing XML+Java, edit visually, write back). Decision: round-trip remains the goal but **v1 is model to generate only**. Importer order later: XML first (easy), Java last (hard, tree-sitter-java). Generated files carry marker comments so an importer has anchors.
