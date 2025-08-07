include(enable_sanitizers)

function(create_executable target)
    set(current_dir "${CMAKE_CURRENT_LIST_DIR}")

    file(GLOB sources CONFIGURE_DEPENDS
        "${current_dir}/*.cpp"
    )

    message(STATUS "[create_executable] Creating target: ${target}")
    message(STATUS "[create_executable] From directory: ${current_dir}")
    message(STATUS "[create_executable] Source files found:")
    
    foreach(src IN LISTS sources)
        message(STATUS "  ---> ${src}")
    endforeach()

    if(ARGN)
        message(STATUS "[create_executable] Linking with: ${ARGN}")
    endif()

    add_executable(${target} ${sources})

    target_link_libraries(${target} PRIVATE ${ARGN})

    enable_sanitizers(${target})
endfunction()
