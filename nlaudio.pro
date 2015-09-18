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
    audioconfig.cpp \
    audiofactory.cpp \
    rawmididevice.cpp \
    examples.cpp

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
    audioconfig.h \
    audiofactory.h \
    midi.h \
    rawmididevice.h \
    examples.h

OTHER_FILES += \
    Todo.txt

