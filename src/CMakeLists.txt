cmake_minimum_required(VERSION 3.27)
project(GDALWARP)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/main/gdal/include)
include_directories(/usr/local/main/proj/include)

link_directories(/usr/local/main/gdal/lib)
link_directories(/usr/local/main/proj/lib)

file(GLOB_RECURSE SRC_CPP_LIST ./src/*)

add_executable(GDALWARP ${SRC_CPP_LIST})

target_link_libraries(GDALWARP gdal proj)
