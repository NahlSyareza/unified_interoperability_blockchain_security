## Build command

```cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

Required libraries:
- mosquitto
- simpleble
- openssl
- crypto

Provided libraries:
- spdlog
- nlohmann-json
- cpp-httplib
