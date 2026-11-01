# core/logger

Implementation of `core/Logger.h`: `debug`/`info`/`warning`/`error` with one
process-wide level, timestamped, written to stdout.

No `<format>`: GCC 12 does not ship it, so a `std::ostringstream` plus
`strftime` keeps the build portable. The level is a mutable global; the Phase 3
UI will own the sink.
