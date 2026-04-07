cmake_minimum_required(VERSION 3.22)
project(Deer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(nlohmann_json REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(deer src/main.cpp)

target_include_directories(deer PRIVATE include)
target_link_libraries(deer PRIVATE Eigen3::Eigen nlohmann_json::nlohmann_json ZLIB::ZLIB)

install(TARGETS deer DESTINATION /usr/local/bin)