cmake_minimum_required(VERSION 3.10)
project(Omnimagnet_Project)

find_package(OpenCV REQUIRED)

add_library(
    omnimagnet
    omnimagnet.cpp
    omnimagnet.hpp
)

add_library(
    type
    type.cpp
    type.hpp
)

add_executable(SystemRun main.cpp)

# Include paths
INCLUDE_DIRECTORIES(/home/ashkan/eigen/source /usr/local/lib)

# Link libraries
target_link_libraries(SystemRun PRIVATE omnimagnet type "/usr/local/lib/libcomedi.a" ${OpenCV_LIBS})
