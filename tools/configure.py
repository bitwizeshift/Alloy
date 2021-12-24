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
                      source_path: Path,
                      defines: Dict[str, str],
                      options: Dict[str, str]):
    # Configure the project
    import subprocess

    command = ["cmake", str(source_path.resolve())]
    command += [f'-D{x}={y}' for x, y in defines.items()]
    command += [f'{x}{y}' for x, y in options.items()]

    process = subprocess.run(command, cwd=build_path)
    return process.returncode


def main() -> int:

    import argparse
    argument_parser = argparse.ArgumentParser(
        description="Configures the project using CMake",
        usage=str(Path(sys.argv[0]).name) + " [options]"
    )

    config_options = argument_parser.add_argument_group(
        "configuration options"
    )
    config_options.add_argument(
        "--all",
        help="Compiles all optional configurations",
        action="store_true",
        default=False,
    )
    config_options.add_argument(
        "--tests",
        help="Compile unit and self-containment tests",
        action="store_true",
        default=False,
    )
    config_options.add_argument(
        "--extras",
        help="Compile extra projects",
        action="store_true",
        default=False,
    )
    config_options.add_argument(
        "--examples",
        help="Compile example projects",
        action="store_true",
        default=False,
    )
    config_options.add_argument(
        "--build-type",
        help="The build type for the project to generate",
        type=str,
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        default="Debug",
        metavar="<type>",
    )
    config_options.add_argument(
        "--shared",
        help="Build as a shared library (default)",
        action="store_true",
        default=True,
    )
    config_options.add_argument(
        "--static",
        help="Build as a static library",
        action="store_false",
        dest="shared",
    )
    config_options.add_argument(
        "--generator",
        help="The cmake generator to target",
        type=str,
        action="store",
        default="Ninja",
        metavar="<generator>",
    )
    config_options.add_argument(
        "--compiler",
        help="The compiler to target",
        type=str,
        action="store",
        metavar="<compiler>",
    )

    output_options = argument_parser.add_argument_group("output options")

    output_options.add_argument(
        "--install-dir",
        help="Where to install to",
        type=Path,
        action="store",
        metavar="<path>",
    )
    output_options.add_argument(
        "--build-dir",
        help="The build directory",
        type=Path,
        action="store",
        metavar="<path>",
    )

    # Handle argument parsing

    args = argument_parser.parse_args()

    def to_switch(x: bool): return "On" if x else "Off"

    enable_tests = args.tests or args.all
    enable_extras = args.extras or args.all
    enable_examples = args.examples or args.all

    cmake_defines = {
        "ALLOY_COMPILE_SELF_CONTAINMENT_TESTS": to_switch(enable_tests),
        "ALLOY_COMPILE_TESTS": to_switch(enable_tests),
        "ALLOY_COMPILE_EXTRAS": to_switch(enable_extras),
        "ALLOY_COMPILE_EXAMPLES": to_switch(enable_examples),
        "CMAKE_BUILD_TYPE": args.build_type,
        "BUILD_SHARED_LIBS": to_switch(args.shared)
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
        build_type = f"{args.build_type}".replace(" ", "_")
        build_type = build_type.replace("-", "_").lower()

        generator = f"{args.generator}".replace(" ", "_")
        generator = generator.replace("-", "_").lower()

        compiler = None
        if args.compiler is not None:
            compiler = f"{args.compiler}".replace(" ", "-").lower()

        build_directory_name = "cmake-build"
        if compiler:
            build_directory_name += f"-{compiler}"
        build_directory_name += f"-{generator}"
        is_single_config = (
            generator != "xcode" and
            not generator.startswith("visual") and
            generator != "ninja multi-config"
        )
        if is_single_config:
            build_directory_name+f"-{build_type}"
        build_directory = Path(build_directory_name.lower())

    if build_directory.is_absolute():
        build_path = build_directory
    else:
        build_path = ProjectPaths.repository_root / build_directory

    # Bootstrap the project

    create_directory(build_path)
    return configure_project(
        build_path=build_path,
        source_path=ProjectPaths.repository_root,
        defines=cmake_defines,
        options=cmake_options,
    )


if __name__ == "__main__":
    import sys
    sys.exit(main())
