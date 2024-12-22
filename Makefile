include scripts/Makefile

ARGS=--enable-x11=true --font="Source Code Pro"

all: build test
.PHONY: all

build:
	cmake -GNinja -B ./build/ -DCMAKE_BUILD_TYPE=RelWithDebugInfo  .
	cmake --build ./build/
.PHONY: build

server:
	MIR_X11_VERBOSE_LOG=true ./build/src/server/hydra $(ARGS) --leader="Left Gui" 2>&1 | tee ./build/log
.PHONY: server

server/help:
	./build/src/server/hydra --help
.PHONY: help

server/wayland:
	WAYLAND_DISPLAY=wayland-99 ./build/src/server/hydra --wayland-host=${WAYLAND_DISPLAY} $(ARGS)
.PHONY: server/wayland

server/wayland-debug:
	WAYLAND_DISPLAY=wayland-99 gdb --args ./build/src/server/hydra --wayland-host=${WAYLAND_DISPLAY} $(ARGS)
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
