# pi-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm) # or aarch64

# Specify the cross-compiler (Update path if using 32-bit)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(ENV{PKG_CONFIG_LIBDIR} "/usr/aarch64-linux-gnu/lib/pkgconfig:/usr/lib/aarch64-linux-gnu/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "/usr/aarch64-linux-gnu")

list(APPEND CMAKE_PREFIX_PATH /usr/aarch64-linux-gnu)
list(APPEND CMAKE_PREFIX_PATH /usr/lib/aarch64-linux-gnu)
list(APPEND CMAKE_PREFIX_PATH /usr/include/aarch64-linux-gnu)

set(APPEND CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(APPEND CMAKE_FIND_ROOT_PATH /usr/lib/aarch64-linux-gnu)
set(APPEND CMAKE_FIND_ROOT_PATH /usr/include/aarch64-linux-gnu)

set(CMAKE_LIBRARY_PATH /usr/lib/aarch64-linux-gnu)
set(CMAKE_INCLUDE_PATH /usr/include/aarch64-linux-gnu)
set(OPENSSL_CRYPTO_LIBRARY /usr/lib/aarch64-linux-gnu/libcrypto.so)
set(OPENSSL_INCLUDE_DIR /usr/include/aarch64-linux-gnu)

# set(CMAKE_PREFIX_PATH /usr/lib/aarch64-linux-gnu)
set(CMAKE_LIBRARY_ARCHITECTURE aarch64-linux-gnu)

# Where is the target environment? (Optional, used for libraries)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
