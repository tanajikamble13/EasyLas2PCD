cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(las2pcd)
find_package(PCL 1.1 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})

include_directories(/usr/include)

link_directories(${PCL_LIBRARY_DIRS})

add_definitions(${PCL_DEFINITIONS})

add_executable(las2pcd las2pcd.cpp)

target_link_libraries(las2pcd 
${PCL_LIBRARIES} 
/usr/lib/liblas.so
)
# for ubuntu 14.04 add /usr/lib/liblas_c.so in target link libraries
