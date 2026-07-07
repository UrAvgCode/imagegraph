# ImageGraph

## Prerequisites

install basic development tools
```bash
sudo apt install cmake g++ git
```
```bash
sudo dnf install cmake gcc-c++ git
```

glfw dependencies
```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```
```bash
sudo dnf install wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
```

native file dialog extended dependencies
```bash
sudo apt install libdbus-1-dev
```
```bash
sudo dnf install dbus-devel
```

glad dependencies
```bash
sudo apt install python3-jinja2 libgl-dev
```
```bash
sudo dnf install python3-jinja2 mesa-libGL-devel
```

cuda support (optional)
```text
CUDA 12
cuDNN 8
```

## Build

clone repository
```bash
git clone --recurse-submodules git@github.com:UrAvgCode/imagegraph.git
cd imagegraph
```

configure project
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

build
```bash
cmake --build build -j
```

install
```bash
cmake --install build --prefix install
```
