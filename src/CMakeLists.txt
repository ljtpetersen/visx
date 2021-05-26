# src/CMakeLists.txt
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

# Build the library.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/)

# If VISX_NOEXE is not 1, ON, YES, TRUE, Y, or a non-zero
# number, then build the executable (in GUI).
# Otherwise, do not build the executable.
if (NOT VISX_NOEXE)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gui/)
endif()

