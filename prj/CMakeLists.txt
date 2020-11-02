#
#  CMakeList.txt
#  icorepo
#
#  Created by kimbomm on 2018. 10. 28...
#  Copyright 2018 kimbomm. All rights reserved.
#
cmake_minimum_required(VERSION 3.4)
project(cvtico)


set(CMAKE_C_STANDARD 11)
list(APPEND CMAKE_C_FLAGS "-std=c++11 -O2 -ffast-math -Wno-unused-result")

set(SOURCE_FILES
        cvtico.cpp
)


include_directories(3rdparty/include)
link_directories(3rdparty/staticlib)
add_executable(cvtico ${SOURCE_FILES})
target_link_libraries(cvtico
        opencv_world340
        libjasper
        libjpeg
        libpng
        libprotobuf
        libtiff
        libwebp
        zlib
)

