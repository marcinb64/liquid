from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class LiquidRecipe(ConanFile):
    name = "liquid"
    version = "0.3.2"

    # Optional metadata
    license = "MIT"
    author = "marcinb64@gmail.com"
    url = "https://github.com/marcinb64/liquid"
    description = "C++ microcontroller interface"
    topics = ("utility", "arduino", "microcontroller", "embedded", "avr")

    # Binary configuration
    settings = "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "platform": ["avr", "linux"]}
    default_options = {"shared": False, "fPIC": False, "platform": "avr"}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "CompilerWarnings.cmake", "StaticAnalysis.cmake", "liquid/*", "demo/*"
    generators = "CMakeDeps"

    def requirements(self):
        if (self.options.platform == 'linux'):
            self.requires("catch2/3.1.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.presets_prefix = "conan-" + str(self.options.platform)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"LIQUID_PLATFORM": self.options.platform, "ENABLE_SANITIZERS": "Off"})
        cmake.build(target="liquid")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["liquid"]
