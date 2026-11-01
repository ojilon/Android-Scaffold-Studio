/*
============================================================

Parser Error

ParseError is a plain struct with no behaviour, so this
translation unit only keeps the file for future diagnostics
helpers (severity mapping, coloured CLI output, JSON report
serialisation).

Nothing is linkable from here yet on purpose: keeping the
empty TU means adding a helper later does not require a CMake
change.

============================================================
*/

#include <parser/ParseError.h>
