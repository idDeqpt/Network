cmake_minimum_required(VERSION 3.4.0)

set(PROJECT_NAME network)
set(PATH_TO_JSTYPES "C:/Projects/C++/libraries/JSTypes/include")

project(${PROJECT_NAME} CXX)
include_directories("./include")

set(SOURCE_LIB
	src/Address.cpp
	src/HTTP.cpp
	src/HTTPRequests.cpp
	src/HTTPServer.cpp
	src/ServerSessionData.cpp
	src/TCPClient.cpp
	src/TCPServer.cpp
	src/URL.cpp
	src/Timer.cpp)       

# Включение экспорта всех символов для создания динамической библиотеки
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Создание динамической библиотеки с именем ${PROJECT_NAME}
add_library(${PROJECT_NAME} SHARED ${SOURCE_LIB})

target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_TO_JSTYPES})
target_link_libraries(${PROJECT_NAME} PRIVATE "C:/Projects/C++/libraries/JSTypes/bin/lib/Debug/jstypes.lib")
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