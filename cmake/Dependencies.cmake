message(STATUS "Loading project dependencies...")

# Vendored third-party libraries, pinned in third_party/README.md.
# Their headers are SYSTEM per AGENTS.md: we never want warnings from code we
# do not own, and we never edit them.
#
# Include roots are chosen so the sources' include directives resolve:
#   <nlohmann/json.hpp>   -> third_party/nlohmann/json.hpp
#   <doctest/doctest.h>   -> third_party/doctest/doctest.h
#   <pugixml.hpp>         -> third_party/pugixml/pugixml.hpp
include_directories(SYSTEM
    "${PROJECT_SOURCE_DIR}/third_party"
    "${PROJECT_SOURCE_DIR}/third_party/pugixml"
)

# pugixml is compiled on demand only: the Phase 4 XML importer is its first
# user, and building 350 kB of XML code that nothing calls would slow the
# low-end target machine for no reason.

#later
#include(FetchContent)
#FetchContent_Declare(...)
#FetchContent_MakeAvailable(...)
