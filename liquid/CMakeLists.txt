set(MODULE_ID liquid)

add_library(
    ${MODULE_ID} STATIC
    src/${LIQUID_PLATFORM}/Sys.cpp
    src/${LIQUID_PLATFORM}/UartImpl.cpp
    src/${LIQUID_PLATFORM}/Interrupts.cpp
    src/${LIQUID_PLATFORM}/AdcImpl.cpp
    src/${LIQUID_PLATFORM}/SysTimer.cpp
    src/${LIQUID_PLATFORM}/boards/ArduinoNano.cpp)

target_include_directories(${MODULE_ID} PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src/${LIQUID_PLATFORM}"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${MODULE_ID} PRIVATE project_warnings project_options)


#target_compile_definitions(${MODULE_ID} PUBLIC LIQUID_BOARD_${LIQUID_BOARD})

install(TARGETS ${MODULE_ID})

install(FILES
    src/Adc.h
    src/Interrupts.h
    src/Pwm.h
    src/Reg.h
    src/Sys.h
    src/SysTimer.h
    src/Uart.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid
    )

install(FILES
    src/avr/AdcImpl.h
    src/avr/AvrAdc.h
    src/avr/AvrTimer8.h
    src/avr/AvrTimer16.h
    src/avr/BoardSelector.h
    src/avr/BoardIsrSelector.cpp
    src/avr/Gpio.h
    src/avr/PwmImpl.h
    src/avr/UartImpl.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid/avr
    )

install(FILES
    src/avr/boards/ArduinoNano.h
    src/avr/boards/ArduinoNanoIsr.cpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid/avr/boards
    )

    
if (ENABLE_UTEST_NEVER)
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
