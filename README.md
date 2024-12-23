Hydra
====

Experimental Mir-based compositor

## Dependencies
### Ubuntu
- g++ (os >= 24.10), g++-14
- libmiral-dev
- mir-graphics-drivers-desktop
- xwayland
- libfmt-dev
- libwayland-dev
- libwayland-dev
- libasound2-dev
- libgl1-mesa-dev
- libxext-dev
- libpugixml-dev
- libglibmm-2.4-dev
- libfontconfig1-dev

## Building
### Local
```
make all
```

### .deb builder for Ubuntu (24.04)
```
make builder
make deb
```

For more details about the build targets, see `Makefile` and `CMakeLists.txt`

## Run
### Installed
```
hydra --enable-x11
```
To use a different leader key (by default "Left Gui"), use the `--leader` option:
```
hydra --leader "Escape" --enable-x11
```

See `hydra --help` for more details.

### Development
- Run the server locally:
```
make server
```

- Run the server using the wayland backend:
```
make server/wayland
```
## Documentation
Some development notes in [docs/dev_notes.pdf](docs/dev_notes.pdf)
