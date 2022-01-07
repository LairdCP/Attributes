# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(attr_display_options)

FILE(GLOB app_sources src/main.c
src/cddl/encode_lock.c
src/cddl/decode_lock.c
src/cbor/cbor_common.c
src/cbor/cbor_encode.c
src/cbor/cbor_decode.c
src/dummy_smp.c
src/test*.c)

zephyr_include_directories(${CMAKE_SOURCE_DIR}/src/cddl
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/include/cbor
${CMAKE_SOURCE_DIR}/include/cddl
)

target_sources(app PRIVATE ${app_sources})
