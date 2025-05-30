cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(PROJECT_NAME network_lib_test)
set(PATH_TO_JSTYPES "C:/Projects/C++/libraries/JSTypes/include")
set(PATH_TO_NETWORK "C:/Projects/C++/libraries/Network/include")

project(${PROJECT_NAME} CXX)

add_executable(${PROJECT_NAME} test.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_TO_JSTYPES})
target_link_libraries(${PROJECT_NAME} PRIVATE "C:/Projects/C++/libraries/JSTypes/bin/lib/Debug/jstypes.lib")

target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_TO_NETWORK})
target_link_libraries(${PROJECT_NAME} PRIVATE "C:/Projects/C++/libraries/Network/bin/lib/Debug/network.lib")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
SET(EXECUTABLE_OUTPUT_PATH ${dir})
SET(LIBRARY_OUTPUT_PATH ${dir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})