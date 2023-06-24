cmake_minimum_required (VERSION 3.11)
 
project (nativefiledialog C CXX)
 
# Collect sources into the variable SOURCE_FILES, HEADER_FILES
set (HEADER_FILES "nativefiledialog/src/nfd_common.h")
set (SOURCE_FILES "nativefiledialog/src/nfd_common.c")
if (WIN32)
    #do something
	message(STATUS ">>> NFD is created for Windows system")
	set (SOURCE_FILES ${SOURCE_FILES} "nativefiledialog/src/nfd_win.cpp")
elseif (APPLE)
    #do something
	message(STATUS ">>> NFD is created for Mac OS system")
	set (SOURCE_FILES ${SOURCE_FILES} "nativefiledialog/src/nfd_cocoa.m")
elseif(UNIX AND NOT APPLE)
    #do something
	message(STATUS ">>> NFD is created for Linux system")
	set (SOURCE_FILES ${SOURCE_FILES} "nativefiledialog/src/nfd_gtk.c")
endif()

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
# source_group("" FILES ${SOURCE_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

include_directories ("nativefiledialog/src/include")
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
add_library (${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
# Creates a folder "nfd" and adds target 
# project (app.vcproj) under it
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})
