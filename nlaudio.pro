TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -pg -pthread
QMAKE_CXXFLAGS += -std=c++11 -pg -pthread
QMAKE_LFLAGS += -pg -pthread
LIBS += -lasound
LIBS += -lpthread
LIBS += -ljack
LIBS += -laubio -L/usr/local/lib64/
LIBS += -lvamp-hostsdk
LIBS += -lvamp-sdk

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
    audiojackinput.cpp \
    audiojack.cpp \
    vamp-simple-host.cpp \
    vamphost.cpp \
    vamphostaubiotempo.cpp

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
    nonblockingcircularbuffer.h \
    audiojackinput.h \
    audiojack.h \
    vamphost.h \
    vamphostaubiotempo.h

OTHER_FILES += \
    Todo.txt

