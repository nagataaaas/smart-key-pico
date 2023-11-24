cmake_minimum_required(VERSION 3.12)
set( CMAKE_CXX_COMPILER "C:/tools/msys64/mingw64/bin/g++.exe" )
set( CMAKE_CXX_COMPILER "C:/tools/msys64/mingw64/bin/gcc++.exe" )
set( PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")
# set(OPENSSL_CRYPTO_LIBRARY "C:/tools/msys64/mingw32/lib/libssl.a")
# set(OPENSSL_INCLUDE_DIR "C:/tools/msys64/mingw32/include")
# set(OPENSSL_ROOT_DIR "C:/tools/msys64/mingw32/include/openssl")
# set( PICO_SDK_PATH "/home/ubuntu20/pico/pico-sdk")
# set(OPENSSL_CRYPTO_LIBRARY "/usr/lib/x86_64-linux-gnu/libssl.a")
# set(OPENSSL_INCLUDE_DIR "/usr/include")
# set(OPENSSL_ROOT_DIR "/usr/include/openssl")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

include_directories("C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/gcc-arm-none-eabi/arm-none-eabi/include")

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

# find_package(OpenSSL REQUIRED)

# Hardware-specific examples in subdirectories:
add_subdirectory(smart_key)