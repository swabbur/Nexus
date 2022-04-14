from conans import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class NexusConan(ConanFile):

    # Package info
    name = "nexus"
    version = "1.0"
    license = "MIT"
    url = "https://github.com/swabbur/nexus.git"
    description = "A cross-platform low-level networking library"
    author = "Wessel van de Brug (wvdbrug@gmail.com)"

    # Dependencies
    requires = "catch2/2.13.8"

    # Binary config
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True, "catch2:with_main": True}
    exports_sources = "include/*", "src/*", "CMakeLists.txt"
    cmake_generator = "Ninja"

    def config_options(self):
        # noinspection PyUnresolvedReferences
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self, generator=self.cmake_generator)

    def generate(self):
        dependencies = CMakeDeps(self)
        dependencies.generate()
        toolchain = CMakeToolchain(self, generator=self.cmake_generator)
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["nexus"]
