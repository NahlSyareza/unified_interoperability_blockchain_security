cmake_minimum_required(VERSION 3.15)
project(legendary_boss)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("./include")
link_directories("./lib")
# include_directories("C:/vcpkg/installed/x64-mingw-static/include")
# link_directories("C:/vcpkg/installed/x64-mingw-static/lib")

find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-mosquitto CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(legendary_boss main.cpp)

target_link_libraries(legendary_boss PRIVATE 
    Crow::Crow
    nlohmann_json::nlohmann_json
    unofficial::mosquitto::mosquitto
    spdlog::spdlog
    # mosquitto
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32 
    mswsock
    crypt32
    # -lpthread
)