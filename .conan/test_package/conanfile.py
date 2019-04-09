#!/usr/bin/env python
import os
from conans import ConanFile, CMake

class AlloyConanTest(ConanFile):
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()


    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="bin")
        self.copy("*.so", dst="bin", src="bin")


    def test(self):
        os.chdir("bin")
        if str(self.settings.os) in ["Windows", "Linux", "Macos"]:
            self.run("alloy-test-package")
        else:
            self.output.warn("Skipping unit test execution due to cross compiling for {}".format(self.settings.os))
