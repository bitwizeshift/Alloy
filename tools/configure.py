#!/usr/bin/env python3
"""
Script for bootstrapping cmake
"""

from pathlib import Path
from python_utilities.paths import ProjectPaths
from typing import Dict


def create_directory(build_path: Path):
    # Make the build directory
    import os
    os.makedirs(build_path, exist_ok=True)


def configure_project(build_path: Path,
                      defines: Dict[str, str],
                      options: Dict[str, str]):
    # Configure the project
    import subprocess

    command = ["cmake", ".."]
    command += [f'-D{x}={y}' for x, y in defines.items()]
    command += [f'{x}{y}' for x, y in options.items()]

    process = subprocess.run(command, cwd=build_path)
    return process.returncode


def main() -> int:

    import argparse
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
    argument_parser.add_argument(
        "--install-dir",
        help="The install path",
        type=Path,
        action="store"
    )
    argument_parser.add_argument(
        "--build-dir",
        help="The build directory",
        type=Path,
        action="store"
    )

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
    if args.compiler is not None:
        cmake_defines["CMAKE_CXX_COMPILER"] = args.compiler

    if args.install_dir is not None:
        cmake_defines["CMAKE_INSTALL_PREFIX"] = args.install_dir

    cmake_options = {
        "-G": args.generator
    }

    # Build directory will encode the generator, and optionally the build type
    # (when not in multi-config generators)
    if args.build_dir is not None:
        build_directory = args.build_dir
    else:
        build_type = f"{args.build_type}".replace(
            " ", "_").replace("-", "_").lower()

        generator = f"{args.generator}".replace(" ", "_")
        generator = generator.replace("-", "_").lower()

        compiler = None
        if args.compiler is not None:
            compiler = f"{args.compiler}".replace(" ", "-").lower()

        build_directory = "cmake-build"
        if compiler:
            build_directory += f"-{compiler}"
        build_directory += f"-{generator}"
        is_single_config = (
            generator != "xcode" and
            not generator.startswith("visual") and
            generator != "ninja multi-config"
        )
        if is_single_config:
            build_directory+f"-{build_type}"
        build_directory = build_directory.lower()

    build_path = ProjectPaths.repository_root / build_directory

    # Bootstrap the project

    create_directory(build_path)
    return configure_project(build_path, cmake_defines, cmake_options)


if __name__ == "__main__":
    import sys
    sys.exit(main())
