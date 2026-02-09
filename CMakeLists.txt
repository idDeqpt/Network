cmake_minimum_required(VERSION 3.5.0)

set(PROJECT_NAME network)

project(${PROJECT_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(Network_BUILD_TESTS "Build tests" OFF)

set(PUBLIC_HEADER
    include/Network/Address.hpp
    include/Network/HTTP.hpp
    include/Network/HTTPRequests.hpp
    include/Network/ServerSessionData.hpp
    include/Network/TCPClient.hpp
    include/Network/TCPServer.hpp
    include/Network/URL.hpp
    include/Network/Timer.hpp
    include/Network/ThreadPool.hpp)

set(SOURCE_FILES
    src/Address.cpp
    src/HTTP.cpp
    src/HTTPRequests.cpp
    src/ServerSessionData.cpp
    src/TCPClient.cpp
    src/TCPServer.cpp
    src/URL.cpp
    src/Timer.cpp)

if (BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME Network
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NetworkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NetworkConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Network
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NetworkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT NetworkTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/Network
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT NetworkTargets
    FILE NetworkTargets.cmake
    NAMESPACE Network::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Network
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NetworkConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NetworkConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Network
)

export(
    EXPORT NetworkTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/NetworkTargets.cmake"
    NAMESPACE Network::
)