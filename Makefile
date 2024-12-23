.env:; touch $@
include .env

include scripts/Makefile

ARGS?=
CMAKE_ARGS?=

all: build test
.PHONY: all

build:
	cmake -B ./build/ ${CMAKE_ARGS} .
	cmake --build ./build/
.PHONY: build

server:
	MIR_X11_VERBOSE_LOG=true ./build/src/server/hydra $(ARGS) 2>&1 | tee ./build/log
.PHONY: server

server/help:
	./build/src/server/hydra --help
.PHONY: help

server/wayland:
	WAYLAND_DISPLAY=wayland-99 ./build/src/server/hydra --wayland-host=${WAYLAND_DISPLAY} --leader="Escape" $(ARGS)
.PHONY: server/wayland

server/wayland-debug:
	WAYLAND_DISPLAY=wayland-99 gdb --args ./build/src/server/hydra --wayland-host=${WAYLAND_DISPLAY} --leader="Escape" $(ARGS)
.PHONY: server/wayland-debug

test/shell-example:
	./build/test/shell-example
.PHONY: test/shell-example

test/shell-example-debug:
	gdb --args ./build/test/shell-example
.PHONY: test/shell-example-debug

test:
	cd build/test && (ctest || ctest --output-on-failure --rerun-failed)
.PHONY: test

tidy:
	run-clang-tidy -p build -header-filter="hydra/(src|include).*" ./src/* ./include/* -quiet

dist:
	test -d build/dist/.git || git clone --no-origin --single-branch -- $(shell pwd) build/dist/
	cd build/dist && rm -rf .git/rebase-merge; git reset --hard @; \
	git fetch $(shell pwd) && git reset --hard FETCH_HEAD
	rm -rf build/hydra.tar.gz; tar --dereference -czvf build/hydra.tar.gz -C build/dist/ .
.PHONY: dist
