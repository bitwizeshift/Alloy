#!/usr/bin/env python3
"""
Script for bootstrapping cmake
"""

import argparse
from pathlib import Path
argument_parser = argparse.ArgumentParser()
argument_parser.add_argument(
    "--tests",
    help="Compile unit tests",
    action="store_true",
    default=True
)
argument_parser.add_argument(
    "--extras",
    help="Compile unit tests",
    action="store_true",
    default=True
)
argument_parser.add_argument(
    "--examples",
    help="Compile unit tests",
    action="store_true",
    default=True
)
argument_parser.add_argument(
    "--build_type",
    help="The build type for the project to generate",
    type=str,
    choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
    default="Debug"
)
argument_parser.add_argument(
    "--shared",
    help="Build as a shared library",
    action="store_true",
    default=True
)
argument_parser.add_argument(
    "--generator",
    help="The cmake generator to target",
    type=str,
    action="store",
    default="Ninja"
)
argument_parser.add_argument(
    "--compiler",
    help="The compiler to target",
    type=str,
    action="store"
)


def root_path():
    import os

    path = os.path.realpath(__file__)  # ./tools/bootstrap.py
    path = os.path.dirname(path)      # ./tools/
    path = os.path.dirname(path)      # ./

    return Path(path)


def create_directory(build_path: Path):
    # Make the build directory
    import os
    os.makedirs(build_directory, exist_ok=True)


def install_dependencies(build_path: Path):
    # Install Conan
    import subprocess

    command = ['conan', 'install', '..']

    subprocess.run(command, cwd=build_path)


def configure_project(build_path: Path, defines: map, options: map):
    # Configure the project
    import subprocess

    command = ["cmake", ".."]
    command += [f'-D{x}={y}' for x, y in cmake_defines.items()]
    command += [f'{x}{y}' for x, y in cmake_options.items()]

    subprocess.run(command, cwd=build_path)


if __name__ == "__main__":

    # Handle argument parsing

    args = argument_parser.parse_args()

    cmake_defines = {
        "ALLOY_COMPILE_SELF_CONTAINMENT_TESTS": "On" if args.tests else "Off",
        "ALLOY_COMPILE_TESTS": "On" if args.tests else "Off",
        "ALLOY_COMPILE_EXTRAS": "On" if args.extras else "Off",
        "ALLOY_COMPILE_EXAMPLES": "On" if args.examples else "Off",
        "CMAKE_BUILD_TYPE": args.build_type,
        "BUILD_SHARED_LIBS": "On" if args.shared else "Off"
    }
    if args.compiler:
        cmake_defines["CMAKE_CXX_COMPILER"] = args.compiler

    cmake_options = {
        "-G": args.generator
    }

    build_type = f"{args.build_type}".replace(
        " ", "_").replace("-", "_").lower()
    generator = f"{args.generator}".replace(" ", "_").replace("-", "_").lower()
    compiler = None if not args.compiler else f"{args.compiler}".replace(
        " ", "-").lower()

    # Build directory will encode the generator, and optionally the build type
    # (when not in multi-config generators)
    build_directory = "cmake-build"
    if compiler:
        build_directory += f"-{compiler}"
    build_directory += f"-{generator}"
    if generator != "xcode" and not generator.startswith("visual") and generator != "ninja multi-config":
        build_directory+f"-{build_type}"
    build_directory = build_directory.lower()

    build_path = root_path() / build_directory

    # Bootstrap the project

    create_directory(build_path)
    install_dependencies(build_path)
    configure_project(build_path, cmake_defines, cmake_options)
