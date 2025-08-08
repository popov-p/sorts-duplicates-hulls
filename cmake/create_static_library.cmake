include(enable_sanitizers)

function(create_static_library target)
    add_library(${target} STATIC)

    file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/${target}/*.cpp)
    file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/${target}/*.h)
    file(GLOB UI_FORMS ${CMAKE_SOURCE_DIR}/src/${target}/*.ui)

    message(STATUS "${target} CONTENTS: \n \t ${SOURCES} \n \t ${HEADERS} \n \t ${UI_FORMS} \n ")

    target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/include/${target})

    message(STATUS "[create_static_library] For Target ${target} CMAKE_AUTOMOC is set to: ${CMAKE_AUTOMOC}")

    target_sources(${target} PRIVATE ${SOURCES} ${HEADERS} ${UI_FORMS})

    if(ARGN)
        target_link_libraries(${target} PRIVATE ${ARGN})
        message(STATUS "[create_static_library] Linking ${target} with: ${ARGN}")
    endif()

    enable_sanitizers(${target})
endfunction()
