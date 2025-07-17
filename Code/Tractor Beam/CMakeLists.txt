cmake_minimum_required(VERSION 3.10)
project(Omnimagnet_Project)

find_package(OpenCV REQUIRED)

# Add omnimagnet Library
add_library(
    omnimagnet
    omnimagnet.cpp
    omnimagnet.hpp
)

# Add cameratrack Library
add_library(
	cameratrack
	cameratrack.cpp
	cameratrack.hpp
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
INCLUDE_DIRECTORIES(/home/ashkan/eigen/source , /usr/local/lib, /usr/include/spinnaker)

# Link libraries
target_link_libraries(cameratrack PRIVATE  "/usr/lib/libSpinnaker.so" ${OpenCV_LIBS})
target_link_libraries(SystemRun PRIVATE omnimagnet type  cameratrack "/usr/local/lib/libcomedi.a")