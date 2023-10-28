cmake_minimum_required(VERSION 3.20)
project(test_skins_generator)
set(CMAKE_CXX_STANDARD 20)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
include(FetchContent)
FetchContent_Declare(
        googletest
        URL ${CMAKE_CURRENT_SOURCE_DIR}/vendor/googletest-5376968f6948923e2411081fd9372e71a59d8e77.zip
        DOWNLOAD_EXTRACT_TIMESTAMP false
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
add_definitions(-DBOOST_USE_WINAPI_VERSION=0x0600)
file(GLOB SOURCES *.cpp ../src/*.cpp)

message(STATUS "test_skins_generator SOURCES: ${SOURCES}")

# 添加测试可执行文件
add_executable(test_skins_generator
        ${SOURCES}
)
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
target_link_libraries(test_skins_generator
        gtest_main
        skins_generator
)
