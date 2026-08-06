## Project Layout

```
AndroidScaffoldStudio/
│
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── LICENSE
│
├── cmake/
│   ├── CompilerOptions.cmake
│   ├── Dependencies.cmake
│   ├── Warnings.cmake
│   └── Utils.cmake
│
├── docs/
│   ├── Architecture.md
│   ├── CodingStyle.md
│   ├── ProjectModel.md
│   ├── XMLFormat.md
│   ├── JavaGeneration.md
│   ├── JNI.md
│   ├── Designer.md
│   ├── PluginAPI.md
│   ├── BuildSystem.md
│   └── Roadmap.md
│
├── assets/
│   ├── icons/
│   ├── fonts/
│   ├── images/
│   └── themes/
│
├── templates/
│   ├── android/
│   ├── java/
│   ├── xml/
│   ├── cpp/
│   └── gradle/
│
├── plugins/
│
├── third_party/
│
├── tests/
│
├── tools/
│
├── generated/
│
├── samples/
│
└── src/
    │
    ├── app/
    │
    ├── core/
    │   ├── logger/
    │   ├── filesystem/
    │   ├── settings/
    │   ├── project/
    │   ├── templates/
    │   ├── utils/
    │   ├── commands/
    │   └── serialization/
    │
    ├── scaffolder/
    │
    ├── parser/
    │   ├── xml/
    │   ├── java/
    │   └── gradle/
    │
    ├── designer/
    │
    ├── rendering/
    │
    ├── importer/
    │
    ├── codegen/
    │
    ├── plugins/
    │
    └── ui/
```