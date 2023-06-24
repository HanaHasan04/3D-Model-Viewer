cmake_minimum_required (VERSION 3.11)
 
project (imgui CXX)
 
# Collect sources into the variable SOURCE_FILES, HEADER_FILES without
# having to explicitly list each header and source file.
#
# CMake documentation states "We do not recommend using GLOB to collect a
# list of source files from your source tree. If no CMakeLists.txt file
# changes when a source is added or removed then the generated build system
# cannot know when to ask CMake to regenerate".
file (GLOB HEADER_FILES
      "imgui/*.h" )
 
# Collect sources into the variable SOURCE_FILES
file (GLOB SOURCE_FILES
      "imgui/*.cpp")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
# source_group("" FILES ${SOURCE_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})
 
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
add_library (${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
# Creates a folder "imgui" and adds target 
# project (app.vcproj) under it
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})
