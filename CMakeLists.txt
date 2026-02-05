cmake_minimum_required(VERSION 3.15)
project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(legendary_boss main.cpp)

target_include_directories(legendary_boss PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # For Crow
    "C:/msys64/ucrt64/include"                 # For Pacman-installed libs
)

target_link_libraries(legendary_boss PRIVATE 
    "C:/msys64/ucrt64/lib/libmosquitto.dll.a"
    "C:/msys64/ucrt64/lib/libspdlog.dll.a"
    "C:/msys64/ucrt64/lib/libfmt.a"
    "C:/msys64/ucrt64/lib/libssl.dll.a"
    "C:/msys64/ucrt64/lib/libcrypto.dll.a"
    ws2_32 
    mswsock
    crypt32
)

target_link_options(legendary_boss PRIVATE 
    "-static-libstdc++" 
    "-static-libgcc" 
    "-pthread"
)