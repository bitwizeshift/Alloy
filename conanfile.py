#!/usr/bin/env python3

from conans import ConanFile, CMake

class AlloyConan(ConanFile):

    # Package Info
    name = "Alloy"
    version = "0.1.0"
    description = "A game engine"
    url = "https://github.com/bitwizeshift/Alloy"
    author = "Matthew Rodusek <matthew.rodusek@gmail.com>"
    license = "MIT"

    # Sources
    exports = ("LICENSE",
               "doc/licenses")
    exports_sources = ( "lib/*",
                        "bin/*",
                        "extra/*",
                        "cmake/*",
                        "doc/*",
                        "CMakeLists.txt",
                        "LICENSE" )

    # Settings
    settings = "os", "compiler", "build_type", "arch"
    options = {
      "install_docs" : [True,False],
      "shared" : [True,False],
      "extras" : [True,False],
      "examples" : [True,False],
      "exceptions" : [True,False],
      "precision" : ["float", "double"]
    }
    default_options = {
      "install_docs" : False,
      "shared" : False,
      "extras" : False,
      "examples" : False,
      "exceptions" : True,
      "precision" : "float"
    }
    generators = "cmake"

    # Dependencies
    build_requires = ("Catch2/2.7.1@catchorg/stable")
    # requires = ("SDL/2.0.9@/stable")

    def requirements(self):
        if self.options.extras:
            self.requires("fmt/5.3.0@bincrafters/stable", "private")

    def imports(self):
        # Puts the licenses of all dependencies into the install directory
        self.copy(pattern="licenses", dst="licenses", folder=True, ignore_case=True)


    def configure_cmake(self):
        cmake = CMake(self)

        # Features
        cmake.definitions["ALLOY_COMPILE_SELF_CONTAINMENT_TESTS"] = "ON"
        cmake.definitions["ALLOY_COMPILE_TESTS"] = "OFF"
        cmake.definitions["ALLOY_COMPILE_EXTRAS"] = "ON" if self.options.extras else "OFF"
        cmake.definitions["ALLOY_COMPILE_EXAMPLES"] = "ON" if self.options.examples else "OFF"
        cmake.definitions["ALLOY_GENERATE_DOCS"] = "ON" if self.options.install_docs else "OFF"
        cmake.definitions["ALLOY_INSTALL_DOCS"] = "ON" if self.options.install_docs else "OFF"

        # ABI
        cmake.definitions["BUILD_SHARED_LIBS"] = "ON" if self.options.shared else "OFF"
        cmake.definitions["ALLOY_ENABLE_EXCEPTIONS"] = "ON" if self.options.exceptions else "OFF"
        cmake.definitions["ALLOY_PRECISION"] = self.options.precision

        cmake.configure()
        return cmake


    def build(self):
        cmake = self.configure_cmake()
        cmake.build()
        # cmake.test()

        # if we are installing documentation, it needs to be generated
        if self.options.install_docs:
            cmake.build(target="AlloyDocs")


    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

        # Copy this library's license, and
        # all dependencies' licenses
        self.copy(pattern="LICENSE", dst="licenses")
        self.copy(pattern="doc/licenses/*", dst="licenses")


    def package_id(self):
        pass