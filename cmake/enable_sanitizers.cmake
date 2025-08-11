function(enable_sanitizers target)
    if(USE_SANITIZERS)
        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(WARNING "[enable_sanitizers] USE_SANITIZERS=ON, but CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'.\nSanitizers are disabled!")
        else()
            message(STATUS "[enable_sanitizers] Using AddressSanitizer for target: ${target}")
        endif()
    endif()

    target_compile_options(${target} PUBLIC
        $<$<AND:$<CONFIG:Debug>,$<BOOL:${USE_SANITIZERS}>>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak>
    )
    target_link_options(${target} PUBLIC
        $<$<AND:$<CONFIG:Debug>,$<BOOL:${USE_SANITIZERS}>>:-fsanitize=address -fsanitize=leak>
    )
    message(STATUS "[enable_sanitizers] Sanitizer flags: $<$<AND:$<CONFIG:Debug>,$<BOOL:${USE_SANITIZERS}>>:-fsanitize=address -fno-omit-frame-pointer>")

endfunction()
