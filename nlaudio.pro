TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lasound
LIBS += -lpthread

SOURCES += main.cpp \
    tools.cpp \
    audioalsa.cpp \
    audioalsainput.cpp \
    audioalsaoutput.cpp \
    audiofactory.cpp \
    rawmididevice.cpp \
    examples.cpp \
    stopwatch.cpp \
    hw_params.c \
    alsacardidentifier.cpp \
    audioalsaexception.cpp \
    alsacardinfo.cpp \
    alsadeviceinfo.cpp \
    alsasubdeviceinfo.cpp \
    bufferstatistics.cpp \
    rawmidideviceexception.cpp \
    samplespecs.cpp \
    minisynth.cpp \
    effects.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    blockingcircularbuffer.h \
    blockinglinearbuffer.h \
    tools.h \
    audio.h \
    audioalsa.h \
    audioalsainput.h \
    audioalsaoutput.h \
    audiofactory.h \
    midi.h \
    rawmididevice.h \
    examples.h \
    stopwatch.h \
    alsacardidentifier.h \
    audioalsaexception.h \
    alsacardinfo.h \
    alsadeviceinfo.h \
    alsasubdeviceinfo.h \
    bufferstatistics.h \
    rawmidideviceexception.h \
    samplespecs.h \
    lockfreecircularbuffer.h \
    minisynth.h \
    effects.h \
    onepolefilters.h \
    biquadfilters.h \
    cabinet.h \
    tiltfilters.h \
    altbiquadfilters.h \
    smoother.h \
    echo.h

OTHER_FILES += \
    Todo.txt

