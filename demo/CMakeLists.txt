set(COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/mem.cpp)

add_subdirectory(gpio)
add_subdirectory(timer)
add_subdirectory(eeprom)
add_subdirectory(i2c)
