cmake_minimum_required(VERSION 3.0)

project(owd)
set(APPLICATION_NAME "Open Weather Data")
set(APPLICATION_CODENAME "${PROJECT_NAME}")
set(APPLICATION_VERSION_MAJOR 0)
set(APPLICATION_VERSION_MINOR 1)
set(APPLICATION_VERSION_STRING "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}")
set(APPLICATION_BUILD_USER $ENV{USER})
set(APPLICATION_BUILD_HOST $ENV{HOSTNAME})
string(TIMESTAMP APPLICATION_BUILD_DATE %m/%d/%Y)

# Configure header files
configure_file(
    "${PROJECT_SOURCE_DIR}/src/modules/version/version.h.in"
    "${PROJECT_SOURCE_DIR}/src/modules/version/version.h"
)

# Setup configurable options
option(OWD_BUILD_HTTP_LIBUV "Build with libuv-backed HTTP stack" OFF)
option(OWD_BUILD_HTTP_CPPREST "Build with cpprest-backed HTTP stack" OFF)
option(OWD_BUILD_TEST "Build tests" ON)

# Setup project outputs
set(PROJECT_DEPS_PATH "${PROJECT_SOURCE_DIR}/3rdparty/deps")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

# Setup project search path
include_directories("${PROJECT_SOURCE_DIR}/src/modules")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/deps")

# Locate project prerequisites
find_package(Threads REQUIRED)
find_package(OpenSSL 1.0.1 REQUIRED)
find_package(Boost 1.55 REQUIRED COMPONENTS
    system thread regex log chrono program_options)

add_subdirectory("${PROJECT_DEPS_PATH}/cpr")

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CPR_INCLUDE_DIRS})

message("DEBUG >> ${CPR_INCLUDE_DIRS}")

link_directories(${Boost_LIBRARY_DIRS})

set(OPENSSL_VERSION "1.0.1")

message("BOOST_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("BOOST_LIBS_DIR: ${Boost_LIBRARY_DIRS}")
message("BOOST_LIBS: ${Boost_LIBRARIES}")
message("OPENSSL_INCLUDE: ${OPENSSL_INCLUDE_DIR}")
message("OPENSSL_LIBS: ${OPENSSL_LIBRARIES}")
message("THREAD_LIB: ${CMAKE_THREAD_LIBS_INIT}")

# Enable a lot of warnings
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-missing-field-initializers")

add_subdirectory(src/modules)
add_subdirectory(src/commands)

if (OWD_BUILD_TEST)
    enable_testing(true)
    add_subdirectory(src/tests)
endif (OWD_BUILD_TEST)
