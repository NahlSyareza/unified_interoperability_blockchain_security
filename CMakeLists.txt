cmake_minimum_required(VERSION 3.15)

if(WIN32)
    set(VCPKG_PATH $ENV{vcpkg})

    if(NOT VCPKG_PATH)
        message(FATAL_ERROR "Please configure vcpkg inside your Environment Variables")
    endif()

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
endif()

set(CMAKE_BUILD_TYPE "Release")

project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(legendary_boss main.cpp utils.cpp)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

if(WIN32)
    find_package(unofficial-mosquitto CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mosquitto REQUIRED libmosquitto)
endif()

find_package(simpleble CONFIG REQUIRED)

if(MSVC)
    target_compile_options(legendary_boss PRIVATE /utf-8)
endif()

target_include_directories(legendary_boss PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# if(WIN32)
#     target_include_directories(legendary_boss PRIVATE "${VCPKG_PATH}/installed/x64-windows/include")
# endif()

target_link_libraries(legendary_boss PRIVATE
    simpleble::simpleble
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

if(WIN32)
    target_link_libraries(legendary_boss PRIVATE unofficial::mosquitto::mosquitto)
else()
    target_link_libraries(legendary_boss PRIVATE ${mosquitto_LIBRARIES})
    # Add this to tell the linker WHERE to find those libraries
    target_link_directories(legendary_boss PRIVATE ${mosquitto_LIBRARY_DIRS}) 
    target_include_directories(legendary_boss PRIVATE ${mosquitto_INCLUDE_DIRS})
endif()

if(NOT WIN32)
    target_link_options(legendary_boss PRIVATE "-pthread")
endif()
