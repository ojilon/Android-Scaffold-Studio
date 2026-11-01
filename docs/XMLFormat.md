# XML Format

The generator's XML conventions (attribute order, escaping, indentation, blank
lines between siblings, LF line endings) are documented in
[04-codegen-and-templates.md](04-codegen-and-templates.md) under "Layout XML
Conventions".

Model-side mapping (which widget becomes which Android view) is in
[02-model-schema.md](02-model-schema.md).

Reading XML back is Phase 4; `parser::parse_xml` is a stub today and pugixml is
vendored but not compiled.
