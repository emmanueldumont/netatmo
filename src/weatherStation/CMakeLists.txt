cmake_minimum_required(VERSION 2.8.3)


add_executable(weatherStation weatherStation.cpp)
target_link_libraries( weatherStation ${catkin_LIBRARIES})
add_dependencies(weatherStation weatherStation_generate_messages_cpp)
