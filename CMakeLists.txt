cmake_minimum_required(VERSION 3.15)
project(legendary_boss)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("./include")
include_directories("C:/vcpkg/installed/x64-windows/include")

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

file(GLOB PROJECT_SOURCES "*.cpp")

foreach(source_file ${PROJECT_SOURCES})
    get_filename_component(exe_name ${source_file} NAME_WE)
    
    add_executable(${exe_name} ${source_file})
    
    target_link_libraries(${exe_name} PRIVATE 
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        ws2_32 
        mswsock
    )
endforeach()