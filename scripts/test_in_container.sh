#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
"$( dirname "${DIR}" )/scripts/build_container.sh"
docker run --rm -t dhoodlum/yourcss /opt/yourcss/scripts/test.sh
