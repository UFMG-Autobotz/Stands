cmake_minimum_required(VERSION 2.8.3)
project(esteira)

# find_package(catkin REQUIRED)
#find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
  rospy
  roscpp
  std_msgs
  geometry_msgs
)

catkin_package(
 #CATKIN_DEPENDS message_runtime 
)


#include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include ${catkin_INCLUDE_DIRS})

#----------------------- Cont  ------------------------ #

add_executable(bolinhas src/main.cpp lib/rs232_class.cpp lib/rs232.c) 
target_link_libraries(bolinhas ${catkin_LIBRARIES} )#${OpenCV_LIBS}) 

