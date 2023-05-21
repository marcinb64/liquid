set(MODULE_ID sandbox)

add_executable(${MODULE_ID} sandbox.cpp)

target_link_libraries(${MODULE_ID} PRIVATE project_warnings project_options
                                           liquid)

if (${AVR})
    make_hex(${MODULE_ID} ${MODULE_ID}.hex)
endif ()
