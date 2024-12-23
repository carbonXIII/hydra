DEB_FILE=hydra-0.1.1-Linux.deb
TEST_HOST?=
DIST_DIR?=

builder:
	podman compose -f scripts/compose.yaml build --no-cache
.PHONY: builder

deb:
	podman compose -f scripts/compose.yaml run --rm builder
.PHONY: deb

install/deps:
	scripts/ubuntu_deps.sh
.PHONY: deps

sync/deb:
	rsync ./build/deb/${DEB_FILE} ${TEST_HOST}:${DIST_DIR}
	ssh ${TEST_HOST} sudo apt install --reinstall ${DIST_DIR}/${DEB_FILE}
.PHONY: install

sync/dist: dist
	rsync ./build/hydra.tar.gz ${TEST_HOST}:${DIST_DIR}
	ssh ${TEST_HOST} \
	"cd ${DIST_DIR} && \
	mkdir -p hydra && cd hydra && tar -xvf ../hydra.tar.gz && \
	sudo apt install make && \
	sudo make install/deps && \
	CMAKE_ARGS='-DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14' make all"
.PHONY: deps
