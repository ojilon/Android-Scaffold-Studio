# Build System

CMake (>= 3.25) + Ninja. Layout:

```
CMakeLists.txt            root: modules, CLI, ctest registration
CMakePresets.json         default / release / asan
cmake/CompilerOptions.cmake   ASSTUDIO_WERROR, ASSTUDIO_SANITIZE
cmake/Warnings.cmake      target_apply_warnings(): the warning set per target
cmake/Dependencies.cmake  SYSTEM include roots for third_party/
cmake/Utils.cmake         placeholder for install/packaging later
```

Commands:

```sh
cmake -B build -S .                       # or: cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
cmake -B build-release -S . -DASSTUDIO_WERROR=ON     # release gate
cmake -B build-asan -S . -DASSTUDIO_SANITIZE=ON      # ASan + UBSan
```

Rules:

- One static library per module; dependencies via `target_link_libraries` only.
- Third-party include dirs are added `SYSTEM`, so their warnings never reach our
  targets.
- `build*/` and `generated/*` are git-ignored; never edit them by hand.
- The `codegen` target name is keepable on CMake 4.4+ thanks to a guarded
  `CMP0171` policy.
