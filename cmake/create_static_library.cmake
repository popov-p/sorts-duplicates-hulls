include(enable_sanitizers)

function(create_static_library target)
    add_library(${target} STATIC)

    file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/${target}/*.cpp)
    file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/${target}/*.h)

    message(STATUS "${target} CONTENTS: \n \t ${SOURCES} \n \t ${HEADERS} \n")

    target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/include/${target})
    target_sources(${target} PRIVATE ${SOURCES})

    if(ARGN)
        target_link_libraries(${target} PRIVATE ${ARGN})
        message(STATUS "[create_static_library] Linking ${target} with: ${ARGN}")
    endif()

    enable_sanitizers(${target})
endfunction()
