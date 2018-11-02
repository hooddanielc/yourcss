#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd $(dirname "${DIR}")
ib yourcss/yourcss.so
