# nice example taken from:
# https://stackoverflow.com/questions/16398937/cmake-and-finding-other-projects-and-their-dependencies
# for the general use of cmake:
# https://cognitivewaves.wordpress.com/cmake-and-visual-studio/
cmake_minimum_required (VERSION 3.11)
project (MeshViewer C CXX)
# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# You can tweak some common (for all subprojects) stuff here. For example:
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

# set some include dirs for all the subprojects
set(glad_INCLUDE_DIRS "ThirdParty/glad/include")
set(glfw_INCLUDE_DIRS "ThirdParty/glfw/include")
set(glm_INCLUDE_DIRS "ThirdParty/glm")
set(imgui_INCLUDE_DIRS "ThirdParty/imgui")
set(nfd_INCLUDE_DIRS "ThirdParty/nativefiledialog/nativefiledialog/src/include")

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)

# When done tweaking common stuff, configure the components (subprojects).
# NOTE: The order matters! The most independent ones should go first.
add_subdirectory(ThirdParty/glad)  # glad is a static library (depends on nothing)
add_subdirectory(ThirdParty/glfw)  # glfw is a static library (depends on nothing)
add_subdirectory(ThirdParty/imgui) # imgui is a static library (depends on nothing)
add_subdirectory(ThirdParty/nativefiledialog) # nativefiledialog is a static library (depends on nothing)

# openmp support
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# will find opengl on the system and create variables for the location of the static libreries etc...
find_package(OpenGL REQUIRED)
message(STATUS ">>> OpenGL found: ${OPENGL_FOUND}")
message(STATUS ">>> OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
# Collect sources into the variable SOURCE_FILES, HEADER_FILES without
# having to explicitly list each header and source file.
#
# CMake documentation states "We do not recommend using GLOB to collect a
# list of source files from your source tree. If no CMakeLists.txt file
# changes when a source is added or removed then the generated build system
# cannot know when to ask CMake to regenerate".
file (GLOB HEADER_FILES
      "Viewer/include/*.h")
 
# Collect sources into the variable SOURCE_FILES
file (GLOB SOURCE_FILES
      "Viewer/src/*.cpp")
file (GLOB SHADER_FILES
      "Viewer/shaders/*.glsl")
# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
# source_group("" FILES ${SOURCE_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Shader Files" FILES ${SHADER_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

 # Properties->C/C++->General->Additional Include Directories
include_directories ("Viewer/include" 
					 ${glad_INCLUDE_DIRS}
					 ${glfw_INCLUDE_DIRS}
					 ${glm_INCLUDE_DIRS}
					 ${imgui_INCLUDE_DIRS}
					 ${nfd_INCLUDE_DIRS}
					 )
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})
# Creates a folder "MeshViewer" and adds target 
# project (app.vcproj) under it
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

# link subprojects	 
target_link_libraries(${PROJECT_NAME} glad glfw imgui nativefiledialog ${OPENGL_LIBRARIES})
# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj

# put all created libreries and executables in lib and bin directories
# https://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(${PROJECT_NAME} glad glfw imgui nativefiledialog
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# This step is very importent. It makes sure each time the shaders
# are changed they are copied again to the out of source build.
# In other words, it copies the shader files to the build directory.
foreach(shader ${SHADER_FILES})
	get_filename_component(shader_name ${shader} NAME)
	message(STATUS ">>> shader: ${shader_name} is copied to ${CMAKE_CURRENT_BINARY_DIR}/${shader_name}")
	configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR}/${shader_name} COPYONLY)
endforeach()

# Copies the shaders from the build (out-of-source-build) directory to the current
# config dir (bin/Release, bin/Debug etc...).
add_custom_command(
        TARGET 	${PROJECT_NAME}		POST_BUILD
        COMMAND ${CMAKE_COMMAND}	-E copy
                ${SHADER_FILES}
                ${CMAKE_CURRENT_BINARY_DIR}/bin/$<CONFIG>
		# DEPENDS ${SHADER_FILES}
		COMMENT "Copying Shaders"
		VERBATIM
)

# If we use visual studio, makes MeshViewer the startup project.
if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif ()
