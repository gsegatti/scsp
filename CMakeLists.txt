cmake_minimum_required(VERSION 3.16)

project(SPSC VERSION 1.0 LANGUAGES CXX)

# Create the library (assuming SPSC.hpp is part of your library)
add_library(SPSC INTERFACE)

# Specify the standard
target_compile_features(SPSC INTERFACE cxx_std_20)

# Set the include directory for SPSC
target_include_directories(SPSC INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # Assuming SPSC.hpp is directly under 'include' folder
    $<INSTALL_INTERFACE:include>  # This line ensures correct installation paths
)

# Find and link Boost
find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DBOOST_ALL_NO_LIB)  # Disable auto-linking (optional)
endif()

# Create the executable and link SPSC library
add_executable(Example src/Example.cpp)
add_executable(Benchmark src/Benchmark.cpp)

# Link the SPSC library to the executables
find_package(Threads REQUIRED)
target_link_libraries(Example PRIVATE SPSC Threads::Threads)
target_link_libraries(Benchmark PRIVATE SPSC Threads::Threads ${Boost_LIBRARIES})

# Install
# include(GNUInstallDirs)
# include(CMakePackageConfigHelpers)

# write_basic_package_version_file(
#     "${CMAKE_CURRENT_BINARY_DIR}/SPSCConfigVersion.cmake"
#     COMPATIBILITY SameMajorVersion
# )

# install(
#     DIRECTORY "include/"
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# install(
#     TARGETS Example
#     DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# install(
#     EXPORT SPSC
#     NAMESPACE SPSC::
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SPSC"
# )

# install(
#     FILES "${CMAKE_CURRENT_BINARY_DIR}/SPSCConfigVersion.cmake"
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SPSC"
# )
