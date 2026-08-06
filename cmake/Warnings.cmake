function(target_apply_warnings TARGET_NAME)

    if(MSVC)

        target_compile_options(
            ${TARGET_NAME}
            PRIVATE
            /W4
        )

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
            #can add -Werror later
        )

    endif()

endfunction()