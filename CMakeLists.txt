# CMakeLists.txt
#
# This file is part of the VisX project (https://github.com/ljtpetersen/visx).
# Copyright (c) 2021 James Petersen
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

project(visx C CXX)
cmake_minimum_required(VERSION 3.1)

# Set the default build type to release.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (JP_CCOMPAT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJP_CCOMPAT")
endif()

# Set the output directory for the binaries and shared and static libraries.
# Only do this if the current directory is the top directory (this project is standalone,
# not part of a larger project).
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
if (CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/debug/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/debug/)
else()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/release/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/release/)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/static/)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR OR JP_DOTESTS)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)
endif()

