## Build command

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

(For Windows OS, do this from inside **Developer Command Prompt for Visual Studio**)

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

Recommended installation via vcpkg (for Windows OS)

[vcpkg GitHub](https://github.com/microsoft/vcpkg)

All libraries can be downloaded except mosquitto and simpleble for Linux-based OS. (Build from source for mosquitto and simpleble)

## Windows Setup
1. Clone vcpkg from it's GitHub
2. Open **Developer Command Prompt for Visual Studio**
3. Navigate to your cloned vcpkg folder
4. Install the following libraries:
```bash
vcpkg install simpleble mosquitto openssl
```
5. Clone this project
6. Open this project inside **Developer Command Prompt for Visual Studio**
7. Run the build command provided at the top
