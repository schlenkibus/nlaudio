cmake_minimum_required(VERSION 3.2)
project(nlaudio VERSION 1.0)

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


