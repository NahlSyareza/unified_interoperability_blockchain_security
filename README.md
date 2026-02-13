## Build command

```cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

Required libraries:
- mosquitto (with pkgconfig for Linux-based OS)
- simpleble
- openssl
- crypto

Provided libraries (header only):
- spdlog
- nlohmann-json
- cpp-httplib
- simpleble utils

Recommended installation via vcpkg (for Windows OS).

[vcpkg GitHub](https://github.com/microsoft/vcpkg)

All libraries can be downloaded except mosquitto and simpleble for Linux-based OS.
Build from source for mosquitto and simpleble.
