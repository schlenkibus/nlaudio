##################
#### Examples ####
##################

cmake_minimum_required(VERSION 3.2)
project(nlaudio_examples)


## inputtooutput
set(INPUTTOOUTPUT_EXAMPLE_SOURCES inputtooutput.cpp)
add_executable(inputtooutput_example ${INPUTTOOUTPUT_EXAMPLE_SOURCES})
target_link_libraries(inputtooutput_example nlaudio)

## silence
set(SILENCE_EXAMPLE_SOURCES silence.cpp)
add_executable(silence_example ${SILENCE_EXAMPLE_SOURCES})
target_link_libraries(silence_example nlaudio)

## midisine
set(MIDISINE_EXAMPLE_SOURCES midisine.cpp)
add_executable(midisine_example ${MIDISINE_EXAMPLE_SOURCES})
target_link_libraries(midisine_example nlaudio)


