include scripts/.local.env

DEB_FILE=hydra-0.1.1-Linux.deb

builder:
	podman compose -f scripts/compose.yaml build --no-cache

deb:
	podman compose -f scripts/compose.yaml run --rm builder

install:
	rsync ./build/deb/${DEB_FILE} ${TEST_HOST}:${DIST_DIR}
	ssh ${TEST_HOST} sudo apt install ${DIST_DIR}/${DEB_FILE}
