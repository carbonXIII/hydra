Hydra
====

Experimental Mir-based compositor

## Dependencies
### Ubuntu
- libmiral-dev (build) or libmiral5t64 (runtime)
- mir-graphics-drivers-desktop
- xwayland (optional: X11 support)

### Arch
- (AUR) mir-git
- xwayland

## Building
### Local
```
make all
```

### Debian
```
make builder
make deb
```

For more details about the build targets, see `Makefile` and `CMakeLists.txt`

## Run
### Installed
```
hydra
```

### Development
```
make server
```

