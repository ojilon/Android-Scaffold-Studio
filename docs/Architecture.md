# Architecture

Moved to a single source: **[01-architecture.md](01-architecture.md)**.

That file has the pipeline, the module table, the dependency rule that CMake
enforces (`core <- parser <- validator <- codegen <- scaffolder <- app`), the
on-disk file layout, and the generated / user-owned split.
