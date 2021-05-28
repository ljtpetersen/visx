# src/gui/CMakeLists.txt
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

set(VISX_CPP_SOURCES "main.cpp")

if (APPLE)
# macOS bundle stuff yay.
add_executable(VisX MACOSX_BUNDLE ${VISX_CPP_SOURCES} icon.icns)
set_target_properties(VisX PROPERTIES
	BUNDLE TRUE
	MACOSX_BUNDLE_GUI_IDENTIFIER ca.jamespetersen.VisX
	MACOSX_BUNDLE_BUNDLE_NAME VisX
	MACOSX_BUNDLE_VERSION "0.1"
	MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
	MACOSX_BUNDLE_ICON_FILE "icon.icns"
	MACOSX_BUNDLE_COPYRIGHT "Copyright (C) 2021 James Petersen"
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/info.plist.in")
set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
target_link_libraries(VisX lvisx wx::net wx::core wx::base)
elseif(WIN32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
add_executable(visx ${VISX_CPP_SOURCES} win_icon.o)
target_link_libraries(visx lvisx wx::net wx::core wx::base)
else()
add_executable(visx ${VISX_CPP_SOURCES})
target_link_libraries(visx lvisx wx::net wx::core wx::base)
endif()

