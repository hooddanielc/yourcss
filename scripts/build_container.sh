#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
docker build -t dhoodlum/yourcss "$( dirname "${DIR}" )"
