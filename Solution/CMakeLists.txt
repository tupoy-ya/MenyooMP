cmake_minimum_required(VERSION 3.20)

include(cmake/crosscompile.cmake)

project(Menyoo)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/source)


message("\nFetching modules")
include(cmake/pugixml.cmake)
include(cmake/dirent.cmake)
include(cmake/simpleini.cmake)
include(cmake/json.cmake)
include(cmake/shv.cmake)


# Menyoo
message(STATUS "Menyoo")
file(GLOB_RECURSE SRC_MAIN
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.h"
    "${SRC_DIR}/**.cpp"
    "${SRC_DIR}/**.cc"
    "${SRC_DIR}/**.cxx"
    "${SRC_DIR}/**.rc"
)

add_library(Menyoo MODULE ${SRC_MAIN})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET Menyoo PROPERTY CXX_STANDARD 20)

source_group(TREE ${SRC_DIR} PREFIX "source" FILES ${SRC_MAIN} )

target_include_directories(Menyoo PRIVATE 
    ${SRC_DIR}
    ${dirent_SOURCE_DIR}/include
    ${simpleini_SOURCE_DIR}
    ${json_SOURCE_DIR}/single_include
    ${shv_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/vendor
)

target_precompile_headers(Menyoo PRIVATE "${SRC_DIR}/common.h")
target_link_libraries(Menyoo PRIVATE pugixml shv winmm shlwapi)

set_target_properties(Menyoo PROPERTIES SUFFIX ".asi")

add_compile_definitions(Menyoo _CRT_SECURE_NO_WARNINGS)
add_compile_definitions(Menyoo MENYOO_CURRENT_VER_="1.18.1")
add_compile_definitions(Menyoo GAME_PLAYERCOUNT=32)

if( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    target_compile_options(Menyoo PRIVATE /wd4244 /wd4305)
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    target_compile_options(Menyoo PRIVATE -Wno-microsoft-abstract)
endif()
