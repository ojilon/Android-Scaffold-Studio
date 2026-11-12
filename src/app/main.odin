package main

import "core:fmt"
import "core:os"
import "core:path/filepath"
import "core:strings"

import "src:parser"
import "src:validator"
import "src:codegen"

// Minimal CLI - Phase 1 parity with the C++ tool.
//
//   asstudio validate <model.json>
//   asstudio generate <model.json> -o <outdir>
//   asstudio --help

main :: proc() {
    args := os.args
    if len(args) < 2 {
        print_help()
        os.exit(1)
    }

    cmd := args[1]
    switch cmd {
    case "validate":
        if len(args) < 3 {
            fmt.eprintln("usage: asstudio validate <model.json>")
            os.exit(1)
        }
        do_validate(args[2])
    case "generate":
        model_path, out_dir := "", ""
        i := 2
        for i < len(args) {
            if args[i] == "-o" && i+1 < len(args) {
                out_dir = args[i+1]
                i += 2
                continue
            }
            if model_path == "" {
                model_path = args[i]
            }
            i += 1
        }
        if model_path == "" || out_dir == "" {
            fmt.eprintln("usage: asstudio generate <model.json> -o <outdir>")
            os.exit(1)
        }
        do_generate(model_path, out_dir)
    case "--help", "-h", "help":
        print_help()
    case:
        fmt.eprintf("unknown command: %s\n", cmd)
        print_help()
        os.exit(1)
    }
}

print_help :: proc() {
    fmt.println("Android Scaffold Studio (Odin) - Phase 1 CLI")
    fmt.println()
    fmt.println("Usage:")
    fmt.println("  asstudio validate <model.json>")
    fmt.println("  asstudio generate <model.json> -o <outdir>")
    fmt.println("  asstudio --help")
    fmt.println()
    fmt.println("The model is the single source of truth. validate checks Android")
    fmt.println("rules (V001...); generate emits XML + Java (+ optional Odin/JNI).")
}

do_validate :: proc(path: string) {
    model, err, ok := parser.load_from_file(path)
    if !ok {
        fmt.eprintf("parse error at %s: %s\n", err.path, err.message)
        os.exit(1)
    }
    defer parser.destroy_model(&model)

    violations := validator.validate(model)
    defer delete(violations)

    if len(violations) == 0 {
        fmt.printf("valid (%d screen(s))\n", len(model.screens))
        os.exit(0)
    }

    for v in violations {
        fmt.eprintf("[%s] %s: %s\n", v.id, v.path, v.message)
    }
    os.exit(1)
}

do_generate :: proc(model_path, out_dir: string) {
    model, err, ok := parser.load_from_file(model_path)
    if !ok {
        fmt.eprintf("parse error at %s: %s\n", err.path, err.message)
        os.exit(1)
    }
    defer parser.destroy_model(&model)

    violations := validator.validate(model)
    defer delete(violations)
    if len(violations) > 0 {
        for v in violations {
            fmt.eprintf("[%s] %s: %s\n", v.id, v.path, v.message)
        }
        fmt.eprintln("refusing to generate: model is invalid")
        os.exit(1)
    }

    // Templates dir: look relative to the binary / cwd
    templates_dir := "templates"

    files, gen_err := codegen.generate_all(model, templates_dir)
    if gen_err != "" {
        fmt.eprintf("codegen error: %s\n", gen_err)
        os.exit(1)
    }
    defer {
        for f in files do delete(f.content)
        delete(files)
    }

    // Ensure output root exists
    if !os.exists(out_dir) {
        os.make_directory_all(out_dir)
    }

    written := 0
    skipped := 0
    for f in files {
        full := filepath.join({out_dir, f.path})
        defer delete(full)

        dir := filepath.dir(full)
        defer delete(dir)
        if !os.exists(dir) {
            os.make_directory_all(dir)
        }

        if f.user_owned && os.exists(full) {
            fmt.printf("skip (user-owned exists): %s\n", f.path)
            skipped += 1
            continue
        }

        if !os.write_entire_file(full, transmute([]byte)f.content) {
            fmt.eprintf("failed to write %s\n", full)
            os.exit(1)
        }
        fmt.printf("wrote %s\n", f.path)
        written += 1
    }

    fmt.printf("done: %d written, %d skipped\n", written, skipped)
}
