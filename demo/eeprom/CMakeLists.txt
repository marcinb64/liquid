set(MODULE_ID eeprom_demo)

add_executable(${MODULE_ID}
    EepromDemo.cpp
    ${COMMON_SOURCES})

target_include_directories(${MODULE_ID} PRIVATE ../common)

target_compile_definitions(${MODULE_ID} PUBLIC F_CPU=${F_CPU})

target_link_libraries(${MODULE_ID} 
    PRIVATE 
        project_warnings 
        project_options 
        liquid)
