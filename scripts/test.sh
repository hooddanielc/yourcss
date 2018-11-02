#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
TEST_DIR="$( dirname "${DIR}" )/test"
cd "$( dirname "${DIR}" )"
ib --test_all test
