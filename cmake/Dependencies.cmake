message(STATUS "Loading project dependencies...")

# Vendored third-party libraries. Treat their headers as system headers
# per AGENTS.md (no project warnings from third-party code).
#
# Include roots are chosen so the sources' include directives resolve:
#   <nlohmann/json.hpp>   -> third_party/nlohmann/json.hpp
#   <doctest/doctest.h>   -> third_party/doctest/doctest.h
#   <pugixml.hpp>         -> third_party/pugixml/pugixml.hpp
include_directories(SYSTEM
    "${PROJECT_SOURCE_DIR}/third_party"
    "${PROJECT_SOURCE_DIR}/third_party/pugixml"
)

#later
#include(FetchContent)
#FetchContent_Declare(...)
#FetchContent_MakeAvailable(...)
