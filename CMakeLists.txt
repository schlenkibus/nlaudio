cmake_minimum_required(VERSION 3.2)
project(nlaudio VERSION 1.0)


set(NLAUDIO_VERSION_MAJOR 1)
set(NLAUDIO_VERSION_MINOR 0)
set(NLAUDIO_VERSION_PATCH 0)
set(NLAUDIO_VERSION_STRING ${NLAUDIO_VERSION_MAJOR}.${NLAUDIO_VERSION_MINOR}.${NLAUDIO_VERSION_PATCH})






include(FindPkgConfig)
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib64/pkgconfig")

set(LIBS
	alsa
	vamp-sdk
	vamp-hostsdk
	vamp
	aubio
	jack
)

####################

function(addLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  include_directories(${${name}_INCLUDE_DIRS})
  link_directories(${${name}_LIBRARY_DIRS})
endfunction(addLib)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(nlaudio ${${name}_LIBRARIES})
endfunction(linkLib)

##################
include_directories(include)
file(GLOB_RECURSE LIBRARY_SOURCES src/*.cpp)
file(GLOB_RECURSE BINARY_SOURCES examples/*.cpp)


add_library(nlaudio SHARED ${LIBRARY_SOURCES})
target_link_libraries(nlaudio pthread)
set_target_properties(nlaudio PROPERTIES VERSION ${NLAUDIO_VERSION_STRING} SOVERSION ${NLAUDIO_VERSION_MAJOR})


add_executable(nlaudio_example ${BINARY_SOURCES})
target_link_libraries(nlaudio_example nlaudio)

foreach(lib ${LIBS})
  addLib(${lib})
endforeach(lib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

add_compile_options(-Wall)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

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
