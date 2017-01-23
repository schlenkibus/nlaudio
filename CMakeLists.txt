cmake_minimum_required(VERSION 3.2)

set(NLAUDIO_VERSION_MAJOR 1)
set(NLAUDIO_VERSION_MINOR 0)
set(NLAUDIO_VERSION_PATCH 0)
set(NLAUDIO_VERSION_STRING ${NLAUDIO_VERSION_MAJOR}.${NLAUDIO_VERSION_MINOR}.${NLAUDIO_VERSION_PATCH})
project(nlaudio VERSION ${NLAUDIO_VERSION_STRING})


option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_DSPTOOLBOX "Build DSP Toolbox" OFF)
option(BUILD_VAMP "Enable vamp experiments" OFF)

include(FindPkgConfig)

set(LIBS alsa jack)
find_package(Threads) # pthread

# This should probably be a setting on the examples
if (${BUILD_VAMP})
	set(LIBS ${LIBS} vamp-sdk vamp-hostsdk vamp aubio)
	set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib64/pkgconfig")
endif()

####################

function(addLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  include_directories(${${name}_INCLUDE_DIRS})
  link_directories(${${name}_LIBRARY_DIRS})
endfunction(addLib)

function(linkLib name target)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(${target} ${${name}_LIBRARIES})
endfunction(linkLib)

################## libnlaudio

set(LIBRARY_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/nlaudio)
include_directories(${LIBRARY_INCLUDE_DIRECTORY})
# Why must this be here, see below... thats where id like it
#include_directories(./dsptoolbox)

file(GLOB_RECURSE LIBRARY_SOURCES_AUDIO src/audio/*.cpp)
file(GLOB_RECURSE LIBRARY_SOURCES_COMMON src/common/*.cpp)
file(GLOB_RECURSE LIBRARY_SOURCES_MIDI src/midi/*.cpp)
if (${BUILD_VAMP})
	file(GLOB_RECURSE LIBRARY_SOURCES_VAMP src/vamp/*.cpp)
endif()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
add_library(nlaudio SHARED ${LIBRARY_SOURCES_COMMON})
target_sources(nlaudio PUBLIC ${LIBRARY_SOURCES_AUDIO})
target_sources(nlaudio PUBLIC ${LIBRARY_SOURCES_MIDI})
set_target_properties(nlaudio PROPERTIES VERSION ${NLAUDIO_VERSION_STRING} SOVERSION ${NLAUDIO_VERSION_MAJOR})

foreach(lib ${LIBS})
  addLib(${lib})
endforeach(lib)

foreach(lib ${LIBS})
  linkLib(${lib} nlaudio)
endforeach(lib)

add_compile_options(-Wall)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

######################

if (${BUILD_EXAMPLES})
	include_directories(./examples)
	add_subdirectory(./examples)
endif()

if (${BUILD_DSPTOOLBOX})
	include_directories(./dsptoolbox)
	add_subdirectory(./dsptoolbox)
endif()

###################
# Doxygen

find_package(Doxygen)

if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxyconfig ${CMAKE_CURRENT_BINARY_DIR}/doxyconfig @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxyconfig
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)

#############
# Install

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/nlaudio DESTINATION include)

INSTALL(TARGETS nlaudio
	LIBRARY DESTINATION lib64
)

