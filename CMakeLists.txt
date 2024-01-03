cmake_minimum_required(VERSION 3.24)
project(skins_generator)
# Debug
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH "D:/C++/vslibs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_RELEASE_TYPE "release")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${PROJECT_RELEASE_TYPE}")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
file(GLOB HEADER_FILES include/*.h)
file(GLOB SOURCE_FILES src/*.cpp)
message(STATUS "HEADER_FILES: ${HEADER_FILES}")
message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
file(REMOVE_RECURSE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resource")
file(COPY
        "${CMAKE_CURRENT_SOURCE_DIR}/resource"
        DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# BOOST
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_DEBUG ON)
#set(Boost_NO_WARN_NEW_VERSIONS TRUE)
add_definitions(-DBOOST_USE_WINAPI_VERSION=0x0600)
find_package(Boost REQUIRED COMPONENTS log log_setup)
if (Boost_FOUND)
    message(STATUS "Boost library status:")
    message(STATUS "    Boost_VERSION: ${Boost_VERSION}")
    message(STATUS "    Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "    Boost_LIBRARIES: ${Boost_LIBRARIES}")

else ()
    message(FATAL_ERROR "Boost not found.")
endif ()

# zlib
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    message(STATUS "ZLIB version: ${ZLIB_VERSION_STRING}")
else ()
    message(FATAL_ERROR "ZLIB not found.")
endif ()
# libzip
find_package(LIBZIP REQUIRED)
if (LIBZIP_FOUND)
    message(STATUS "LibZip version: ${LIBZIP_VERSION}")
else ()
    message(FATAL_ERROR "LibZip not found.")
endif ()
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#add_subdirectory(test_skins_generator)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
add_executable(skins_generator src/Generator.cpp ${HEADER_FILES} ${SOURCE_FILES})
target_compile_options(skins_generator PUBLIC -O2)

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
target_link_libraries(skins_generator
        ${Boost_LIBRARIES}
        libzip::zip
        ZLIB::ZLIB
)
add_custom_command(
        TARGET skins_generator
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_PREFIX_PATH}/bin/zlib.dll"
        "$<TARGET_FILE:libzip::zip>"
        "$<TARGET_FILE_DIR:skins_generator>"
)

