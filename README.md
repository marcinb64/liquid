# libmist

## About

C++ microcontroller interface.

## Dependencies

 * Catch2 for unit testing

## Build

Example build commands for AVR 2560. Assumes a Conan profile named avr-mega2560 exists.

    conan install conanfile.py --build=missing --output-folder=build.avr-mega2560 -pr:b=default -pr:h=avr-mega2560
    source build.avr-mega2560/build/Release/generators/conanbuild.sh   # Load cross-compile config
    cmake --preset conan-avr-release -DCMAKE_BUILD_TYPE=Release -DLIQUID_PLATFORM=avr -DLIQUID_BOARD=ArduinoMega -DF_CPU=16000000 
    cmake --build build.avr-mega2560/build/Release
