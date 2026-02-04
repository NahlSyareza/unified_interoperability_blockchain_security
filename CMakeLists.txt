cmake_minimum_required(VERSION 3.15)
project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(legendary_boss main.cpp)


target_include_directories(legendary_boss PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "C:/msys64/ucrt64/include"
)

target_link_libraries(legendary_boss PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmosquitto.dll.a"
    
    "C:/msys64/ucrt64/lib/libssl.dll.a"
    "C:/msys64/ucrt64/lib/libcrypto.dll.a"
    
    ws2_32 
    mswsock
    crypt32
)