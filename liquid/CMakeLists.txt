set(MODULE_ID liquid)

add_library(
    ${MODULE_ID} STATIC
    src/${LIQUID_PLATFORM}/Sys.cpp
    src/${LIQUID_PLATFORM}/UartImpl.cpp
    src/${LIQUID_PLATFORM}/Interrupts.cpp
    src/${LIQUID_PLATFORM}/AdcImpl.cpp
    src/${LIQUID_PLATFORM}/SysTimer.cpp
    src/${LIQUID_PLATFORM}/boards/ArduinoNano.cpp
    src/${LIQUID_PLATFORM}/boards/ArduinoMega.cpp)

file(GLOB COMMON_HEADER_FILES src/*.h)
file(GLOB PLATFORM_HEADER_FILES src/${LIQUID_PLATFORM}/*.h)
file(GLOB BOARD_HEADER_FILES src/${LIQUID_PLATFORM}/boards/*.h)

target_include_directories(${MODULE_ID} PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src/${LIQUID_PLATFORM}"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${MODULE_ID} PRIVATE project_warnings project_options)

target_compile_definitions(${MODULE_ID} PUBLIC LIQUID_BOARD_${LIQUID_BOARD})

install(TARGETS ${MODULE_ID})

install(FILES
    ${COMMON_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid)

install(FILES
    ${PLATFORM_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid/${LIQUID_PLATFORM})

install(FILES
    ${BOARD_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid/${LIQUID_PLATFORM}/boards)

    
if (BUILD_TESTING)
    find_package(Catch2 REQUIRED)

    add_executable(utest_${MODULE_ID} test/utest_core.cpp)

    target_include_directories(utest_${MODULE_ID} PRIVATE ".")

    target_link_libraries(
        utest_${MODULE_ID}
        PRIVATE project_warnings
                project_options
                Catch2::Catch2WithMain
                ${MODULE_ID})

    include(CTest)
    include(Catch)
    catch_discover_tests(utest_${MODULE_ID} TEST_PREFIX "${MODULE_ID}@")
endif ()
