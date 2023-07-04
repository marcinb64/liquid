set(MODULE_ID sandbox)

add_executable(${MODULE_ID} sandbox.cpp mem.cpp)

target_compile_definitions(${MODULE_ID} PUBLIC F_CPU=${F_CPU})

target_link_libraries(${MODULE_ID} PRIVATE project_warnings project_options
                                           liquid)
