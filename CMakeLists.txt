cmake_minimum_required(VERSION 3.15)

if(WIN32)
    set(VCPKG_PATH $ENV{vcpkg})

    if(NOT VCPKG_PATH)
        message(FATAL_ERROR "Please configure vcpkg inside your Environment Variables")
    endif()

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
endif()

project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    find_package(unofficial-mosquitto CONFIG REQUIRED)
else()
    find_package(mosquitto REQUIRED)
endif()

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(legendary_boss main.cpp)

target_include_directories(legendary_boss PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
    target_include_directories(legendary_boss PRIVATE "${VCPKG_PATH}/installed/x64-windows-static/include")
    # target_include_directories(legendary_boss PRIVATE "${VCPKG_PATH}/installed/x64-windows/include")
endif()

target_link_libraries(legendary_boss PRIVATE
    # mosquitto::mosquitto
    spdlog::spdlog
    fmt::fmt
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

if(WIN32)
    target_link_libraries(legendary_boss PRIVATE unofficial::mosquitto::mosquitto)
else()
    target_link_libraries(legendary_boss PRIVATE mosquitto)
endif()

if(NOT WIN32)
    target_link_options(legendary_boss PRIVATE "-pthread")
endif()