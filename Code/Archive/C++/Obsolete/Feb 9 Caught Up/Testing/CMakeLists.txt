cmake_minimum_required(VERSION 3.10)
project(Omnimagnet_Project)
# find_package( OpenCV REQUIRED )
add_library(
	omnimagnet
	omnimagnet.cpp
	omnimagnet.hpp
	)
# add_library(
# 	cameratrack
# 	cameratrack.cpp
# 	cameratrack.hpp
# 	)

add_library(
	type
	type.cpp
	type.hpp
	)

add_library(
	multimagnet
	multimagnet.cpp
	multimagnet.hpp
	)

add_executable(SystemRun main.cpp)
# INCLUDE_DIRECTORIES("/home/ashkan/Desktop/naghi_C/build/")
INCLUDE_DIRECTORIES(/home/charlie/Desktop/eigen , /usr/local/lib)

# target_link_libraries(pigi PRIVATE "/home/ashkan/Desktop/naghi_C/build/libghazam.a" )
# target_link_libraries(SystemRun PRIVATE omnimagnet type "/usr/lib/libSpinnaker.so")
# target_link_libraries(cameratrack PRIVATE  "/usr/lib/libSpinnaker.so" ${OpenCV_LIBS})
target_link_libraries(SystemRun PRIVATE omnimagnet type multimagnet)
