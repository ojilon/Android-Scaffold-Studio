package core

// Simple Result type used across module boundaries.
// Prefer this over exceptions; the CLI is the only place that exits.

Result :: struct($T: typeid, $E: typeid) {
    ok:  bool,
    val: T,
    err: E,
}

ok :: proc($T: typeid, $E: typeid, v: T) -> Result(T, E) {
    return Result(T, E){ok = true, val = v}
}

err :: proc($T: typeid, $E: typeid, e: E) -> Result(T, E) {
    return Result(T, E){ok = false, err = e}
}

is_ok :: proc(r: $R/Result($T, $E)) -> bool {
    return r.ok
}

is_err :: proc(r: $R/Result($T, $E)) -> bool {
    return !r.ok
}

unwrap :: proc(r: $R/Result($T, $E)) -> T {
    assert(r.ok, "unwrap called on error Result")
    return r.val
}

unwrap_err :: proc(r: $R/Result($T, $E)) -> E {
    assert(!r.ok, "unwrap_err called on ok Result")
    return r.err
}
