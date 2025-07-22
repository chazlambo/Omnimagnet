cmake_minimum_required(VERSION 3.10)
project(Omnimagnet_Project)

find_package(OpenCV REQUIRED)

# Add omnimagnet Library
add_library(
    omnimagnet
    omnimagnet.cpp
    omnimagnet.hpp
)

# Add camera capture Library
add_library(
    cameracapture
    cameracapture.cpp
    cameracapture.hpp
)

# Add type Library
add_library(
    type
    type.cpp
    type.hpp
)

# Add Executable
add_executable(SystemRun main.cpp)

# Include paths
INCLUDE_DIRECTORIES(/home/ashkan/eigen/source /opt/spinnaker/include)
LINK_DIRECTORIES(/opt/spinnaker/lib)

# Link libraries
target_link_libraries(cameracapture PRIVATE "/usr/lib/libSpinnaker.so" ${OpenCV_LIBS})
target_link_libraries(SystemRun PRIVATE omnimagnet type cameratrack "/usr/local/lib/libcomedi.a")