#!/usr/bin/env bash
################################################################################
# Travis-CI : test
# ----------------
#
# Executes the test projects
################################################################################

# TODO(bitwizeshift): Sync binaries from AWS

for bin in $(find build/bin -name "*-test"); do
  ${bin}
done