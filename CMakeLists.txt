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

add_executable(legendary_boss main.cpp src/ble_handler.cpp src/de_ruyter.cpp src/http_handler.cpp src/mqtt_handler.cpp src/spitarm.cpp src/tour_de_scheduler.cpp src/data_structure.cpp src/rf24_handler.cpp src/uart_handler.cpp src/i2c_handler.cpp)

find_path(RF24_INCLUDE RF24.h PATH_SUFFIXES RF24)
find_library(RF24_LIB NAMES rf24)

target_include_directories(legendary_boss PRIVATE ${RF24_INCLUDE})
target_link_libraries(legendary_boss PRIVATE ${RF24_LIB})

if(WIN32)
    find_package(PThreads4W REQUIRED)
    target_link_libraries(legendary_boss PRIVATE PThreads4W::PThreads4W)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(legendary_boss PRIVATE Threads::Threads)
endif()

if(MSVC)
    target_compile_options(legendary_boss PRIVATE /W4 /WX)
else()
    target_compile_options(legendary_boss PRIVATE -Wall -Wextra -Werror)
endif()

# find_package(Threads REQUIRED)
# target_link_libraries(legendary_boss PRIVATE Threads::Threads)

if(WIN32)
    find_package(unofficial-mosquitto CONFIG REQUIRED)
    target_link_libraries(legendary_boss PRIVATE unofficial::mosquitto::mosquittopp)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mosquitto REQUIRED libmosquitto)
    target_link_libraries(legendary_boss PRIVATE ${mosquitto_LIBRARIES})
    # target_link_directories(legendary_boss PRIVATE ${mosquitto_LIBRARY_DIRS}) 
    target_include_directories(legendary_boss PRIVATE ${mosquitto_INCLUDE_DIRS})
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries(legendary_boss PRIVATE OpenSSL::SSL)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE spdlog::spdlog)

find_package(simplebluez CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE simplebluez::simplebluez)

add_compile_definitions(FMT_HEADER_ONLY)

if(WIN32)
    find_package(httplib CONFIG REQUIRED)
    target_link_libraries(legendary_boss PRIVATE httplib::httplib)
else()
    find_package(PkgConfig REQUIRED)
pkg_check_modules(HTTPLIB REQUIRED IMPORTED_TARGET cpp-httplib)
    target_link_libraries(legendary_boss PRIVATE PkgConfig::HTTPLIB)
endif()

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE nlohmann_json::nlohmann_json)

if(MSVC)
    target_compile_options(legendary_boss PRIVATE /utf-8)
endif()

target_include_directories(legendary_boss PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# if(NOT WIN32)
target_link_options(legendary_boss PRIVATE "-pthread")
# endif()
