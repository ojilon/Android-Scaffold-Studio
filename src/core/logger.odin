package core

import "core:fmt"
import "core:time"

Level :: enum {
    Debug,
    Info,
    Warn,
    Error,
}

@(private)
current_level: Level = .Info

set_level :: proc(l: Level) {
    current_level = l
}

log :: proc(level: Level, format: string, args: ..any) {
    if level < current_level do return
    prefix := level_prefix(level)
    now := time.now()
    ts := time.to_string_hms(now)
    msg := fmt.tprintf(format, ..args)
    fmt.printf("[%s] %s %s\n", ts, prefix, msg)
}

debug :: proc(format: string, args: ..any) { log(.Debug, format, ..args) }
info  :: proc(format: string, args: ..any) { log(.Info,  format, ..args) }
warn  :: proc(format: string, args: ..any) { log(.Warn,  format, ..args) }
error :: proc(format: string, args: ..any) { log(.Error, format, ..args) }

@(private)
level_prefix :: proc(l: Level) -> string {
    switch l {
    case .Debug: return "DBG"
    case .Info:  return "INF"
    case .Warn:  return "WRN"
    case .Error: return "ERR"
    }
    return "???"
}
