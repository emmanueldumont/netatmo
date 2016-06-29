cmake_minimum_required(VERSION 2.8.3)


add_executable(welcome welcome .cpp)
target_link_libraries( welcome ${catkin_LIBRARIES})
add_dependencies(welcome welcome_generate_messages_cpp)
