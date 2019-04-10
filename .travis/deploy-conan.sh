#!/usr/bin/env bash
################################################################################
# Travis-CI : deploy-conan
# ------------------------
#
# Deploys a CMake project
################################################################################

user="Alloy"
version=$(conan inspect --attribute version | sed 's@version: @@g')

test_path="$(pwd)/.conan/test_package"

conan create . "alloy/stable"
conan test "${user}/${version}@alloy/stable" --test-folder="${test_path}"