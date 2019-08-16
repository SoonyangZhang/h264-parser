PROJECT(project)
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall  -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall  -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")
set(CMAKE_CXX_FLAGS "-fPIC")  
set(CMAKE_C_FLAGS "-fPIC") 
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-DNDEBUG)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/logging)
include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/media/include)
set(media_LIB
media/src/h264_bit_reader.cc
media/src/h264_parser.cc
media/src/media_info.cc
media/src/media_util.cc
)
add_library(media STATIC ${media_LIB})



set(EXECUTABLE_NAME "a_main")
add_executable(${EXECUTABLE_NAME} main.cc)
target_link_libraries(${EXECUTABLE_NAME}  media)

