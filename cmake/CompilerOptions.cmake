message(STATUS "Loading compiler options...")

# Warnings-as-errors for our own code. Kept OFF by default so a newer compiler
# introducing a new warning cannot block a build; release/CI builds turn it on:
#   cmake -B build -DASSTUDIO_WERROR=ON
option(ASSTUDIO_WERROR "Treat project warnings as errors" OFF)

# AddressSanitizer + UBSan (GCC/Clang only), per AGENTS.md "Optional".
option(ASSTUDIO_SANITIZE "Build with AddressSanitizer and UBSan" OFF)

if(ASSTUDIO_SANITIZE)
    if(MSVC)
        message(WARNING "ASSTUDIO_SANITIZE is only supported on GCC/Clang; ignoring")
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()
