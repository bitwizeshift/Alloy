#!/usr/bin/env bash
################################################################################
# Travis-CI : deploy-conan
# ------------------------
#
# Builds and deploys a conan project
################################################################################

set -e

test_path="$(pwd)/.conan/test_package"
conanfile_path="$(pwd)"

user="Alloy"
version=$(conan inspect "${conanfile_path}" --attribute version | sed 's@version: @@g')

conan create . "alloy/stable"
conan test "${user}/${version}@alloy/stable" --test-folder="${test_path}"