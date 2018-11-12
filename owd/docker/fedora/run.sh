# !/bin/bash

set -xe

docker build -t owd_fedora docker/fedora
docker run \
    --rm -ti \
    -u ${UID} \
    -w /opt/owd \
    -v ${PWD}:/opt/owd \
    owd_fedora /bin/bash -c "scripts/build.sh"
