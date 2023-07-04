from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class LiquidRecipe(ConanFile):
    name = "liquid"
    version = "0.3.2"

    # Optional metadata
    license = "MIT"
    author = "marcinb64@gmail.com"
    #url = "http://site.com"
    description = "C++ microcontroller interface"
    topics = ("foundations", "utility", "arduino", "microcontroller", "embedded", "avr")

    # Binary configuration
    settings = "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "platform": ["avr"]}
    default_options = {"shared": False, "fPIC": False, "platform": "avr"}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "CompilerWarnings.cmake", "StaticAnalysis.cmake", "liquid/*", "liquid/src/*", "liquid/src/avr/*", "liquid/src/avr/boards/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"LIB_ONLY": "True", "LIQUID_PLATFORM": self.options.platform, "ENABLE_SANITIZERS": "Off"})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["liquid"]
