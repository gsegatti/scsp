cmake_minimum_required(VERSION 3.16)

project(SPSC VERSION 1.0 LANGUAGES CXX)

add_library(SPSC INTERFACE)

target_compile_features(SPSC INTERFACE cxx_std_20)

target_include_directories(SPSC INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(Example src/Example.cpp)
add_executable(Benchmark src/Benchmark.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Example PRIVATE SPSC Threads::Threads)
target_link_libraries(Benchmark PRIVATE SPSC Threads::Threads ${Boost_LIBRARIES})
