cmake_minimum_required(VERSION 3.10)
project(Omnimagnet_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(OpenCV REQUIRED)
find_library(COMEDI_LIB comedi PATHS /usr/lib /usr/local/lib)
find_library(SPINNAKER_LIB Spinnaker PATHS /opt/spinnaker/lib)

if (NOT COMEDI_LIB)
    message(FATAL_ERROR "libcomedi not found")
endif()
if (NOT SPINNAKER_LIB)
    message(FATAL_ERROR "Spinnaker library not found")
endif()

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    /home/ashkan/eigen/source
    /opt/spinnaker/include
)

# Libraries
add_library(omnimagnet omnimagnet.cpp omnimagnet.hpp)
add_library(cameracapture cameracapture.cpp cameracapture.hpp)
add_library(type type.cpp type.hpp)

# Executable
add_executable(SystemRun main.cpp)

# Linking
target_link_libraries(cameracapture PRIVATE ${SPINNAKER_LIB} ${OpenCV_LIBS})
target_link_libraries(SystemRun PRIVATE omnimagnet type cameracapture ${COMEDI_LIB} pthread)
