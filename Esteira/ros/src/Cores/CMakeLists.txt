cmake_minimum_required(VERSION 2.8.3)
project(Cores)

# find_package(catkin REQUIRED)
find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
  rospy
  roscpp
  std_msgs
  geometry_msgs
)

catkin_package(
 #CATKIN_DEPENDS message_runtime 
)


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include ${catkin_INCLUDE_DIRS})

#----------------------- Cont  ------------------------ #

add_executable(Cores_cont src/continuo_ROS.cpp lib/color_detection.cpp) 
target_link_libraries(Cores_cont ${catkin_LIBRARIES} ${OpenCV_LIBS}) 
#add_dependencies(SVM_use ${catkin_ROS_EXPORTED_TARGETS})

#----------------------- Discr  ------------------------ #

add_executable(Cores_discr src/discreto_ROS.cpp lib/color_detection.cpp)
target_link_libraries(Cores_discr ${catkin_LIBRARIES} ${OpenCV_LIBS}) 
#add_dependencies(SVM_train ${catkin_ROS_EXPORTED_TARGETS})
