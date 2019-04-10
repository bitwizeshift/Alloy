#!/usr/bin/env bash
################################################################################
# Travis-CI : validate
# --------------------
#
# Validates the test projects
################################################################################

# TODO(bitwizeshift): Sync binaries from AWS

for bin in $(find build/bin -name "*-test"); do
  ${bin}
done