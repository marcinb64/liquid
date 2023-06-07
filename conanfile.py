from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class HelloConan(ConanFile):
    name = "liquid"
    version = "0.2.3"

    # Optional metadata
    license = "MIT"
    author = "marcinb64@gmail.com"
    #url = "http://site.com"
    description = "C++ microcontroller interface"
    topics = ("foundations", "utility", "arduino", "microcontroller", "embedded", "avr")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False], "fPIC": [False]}
    default_options = {"shared": False, "fPIC": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "CompilerWarnings.cmake", "StaticAnalysis.cmake", "liquid/*", "liquid/src/*", "liquid/src/avr/*", "liquid/src/avr/boards/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"LIB_ONLY": "True", "LIQUID_PLATFORM": "avr"})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["liquid"]
