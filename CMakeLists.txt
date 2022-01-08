cmake_minimum_required(VERSION 3.0.0)
project(test_project_1 VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(test_project_1 main.cpp)

find_library(LIBRT rt) 
if(LIBRT)
target_link_libraries(test_project_1 ${LIBRT})
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
