function(target_apply_warnings TARGET_NAME)

    if(MSVC)

        target_compile_options(
            ${TARGET_NAME}
            PRIVATE
            /W4
            /permissive-
            /utf-8
        )

        if(ASSTUDIO_WERROR)
            target_compile_options(${TARGET_NAME} PRIVATE /WX)
        endif()

    else()

        target_compile_options(
            ${TARGET_NAME}
            PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
        )

        if(ASSTUDIO_WERROR)
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()

    endif()

endfunction()
