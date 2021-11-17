# set(CMAKE_C_COMPILER "gcc-7")
# set(CMAKE_CXX_COMPILER "g++-7")

get_filename_component(PROJECT_NAME_STR ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME_STR})

cmake_minimum_required(VERSION 2.8)
project(${PROJECT_NAME_STR})

#----------------------------------------
# set compiler
#----------------------------------------
add_definitions(-Wall -std=c++11)

find_package(Boost 1.62.0)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

#----------------------------------------
# Application
#----------------------------------------
aux_source_directory(. SRC_LIST)

# Headers
file(GLOB HEADERS_LIST "*.h" "*.hpp")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS_LIST})

#----------------------------------------
# Tests
#----------------------------------------
enable_testing()
add_test(tests ${PROJECT_NAME})
