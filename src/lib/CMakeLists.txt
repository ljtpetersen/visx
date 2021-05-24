# src/lib/CMakeLists.txt
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

set(LVISX_CPP_SOURCES "uasf.cpp")

add_library(lvisx STATIC)
cmake_policy(SET CMP0076 NEW)
target_sources(lvisx PUBLIC ${LVISX_CPP_SOURCES})
set_source_files_properties(${LVISX_CPP_SOURCES}
	PROPERTIES COMPILE_OPTIONS "-Wall"
			   LANGUAGE CXX)
