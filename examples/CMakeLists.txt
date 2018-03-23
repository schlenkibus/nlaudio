##################
#### Examples ####
##################

cmake_minimum_required(VERSION 3.2)
project(nlaudio_examples)

### libnlaudio
set(NLAUDIO_LIBRARY_PATH /usr/local/lib64)
set(NLAUDIO_LIBRARY ${NLAUDIO_LIBRARY_PATH}/libnlaudio.so)
set(NLAUDIO_INCLUDE_DIR /usr/local/include/nlaudio)

add_compile_options(-Wall)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)

if (NOT EXISTS "${NLAUDIO_LIBRARY}" OR NOT EXISTS "${NLAUDIO_INCLUDE_DIR}")
	set(NLAUDIO_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/../lib)
	set(NLAUDIO_LIBRARY ${NLAUDIO_LIBRARY_PATH}/libnlaudio.so)
	set(NLAUDIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/nlaudio)
endif()

message("Found nlaudio:  ${NLAUDIO_LIBRARY_PATH}")
message("                ${NLAUDIO_LIBRARY}")
message("                ${NLAUDIO_INCLUDE_DIR}")

## inputtooutput
include_directories(${NLAUDIO_INCLUDE_DIR})
link_directories(${NLAUDIO_LIBRARY_PATH})
add_executable(nlaudio_inputtooutput inputtooutput.cpp)
target_link_libraries(nlaudio_inputtooutput nlaudio)

### silence
set(SILENCE_EXAMPLE_SOURCES silence.cpp)
add_executable(silence_example ${SILENCE_EXAMPLE_SOURCES})
target_link_libraries(silence_example nlaudio)

### midisine
set(MIDISINE_EXAMPLE_SOURCES midisine.cpp)
add_executable(midisine_example ${MIDISINE_EXAMPLE_SOURCES})
target_link_libraries(midisine_example nlaudio)

### recorder
set(RECORDER_EXAMPLE_SOURCES recorder.cpp)
add_executable(nlaudio_recorder ${RECORDER_EXAMPLE_SOURCES})
target_link_libraries(nlaudio_recorder nlaudio)

### c15_audio_engine

FILE(GLOB_RECURSE C15_AUDIO_ENGINE_SOURCES "c15_audio_engine/*")

add_executable(c15_audio_engine ${C15_AUDIO_ENGINE_SOURCES} c15_audio_engine.cpp)
target_link_libraries(c15_audio_engine nlaudio)

############
# Install

INSTALL(TARGETS c15_audio_engine
	RUNTIME DESTINATION bin)

