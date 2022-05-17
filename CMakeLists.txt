cmake_minimum_required(VERSION 3.18)
project(dental VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set(architecture "x64")
else()
  set(architecture "x86")
endif()

message(STATUS "${CMAKE_SYSTEM} ${architecture}")

add_compile_options(-g)
add_compile_options(-fpic)
# add_compile_options(-Wall)
# add_compile_options(-Wextra)

set(library_dir ${CMAKE_SOURCE_DIR}/lib)
set(runtime_dir ${CMAKE_SOURCE_DIR}/bin)
set(assets_dir ${CMAKE_SOURCE_DIR}/assets)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${library_dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${library_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${runtime_dir})

configure_file(config.in config.h)

set(ROOT "C:/msys64/mingw64/include")

include_directories(${ROOT})
include_directories(thirdparty)
include_directories(thirdparty/vcglib)
include_directories(include)
include_directories(build)

file(GLOB_RECURSE INCS include/*.h)
file(GLOB_RECURSE SRCS src/*.cpp src/*.c src/*.h)
add_executable(${PROJECT_NAME} ${INCS} ${SRCS})

target_link_libraries(${PROJECT_NAME} glfw3 GLESv2)

build_command(BUILD_COMMAND_LINE CONFIGURATION ${CMAKE_BUILD_TYPE} TARGET ALL_BUILD)
message(STATUS "Using compiler ${CMAKE_CXX_COMPILER}")
message(STATUS "Build command line: ${BUILD_COMMAND_LINE}")