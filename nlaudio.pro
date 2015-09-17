TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lasound
LIBS += -lpthread

SOURCES += main.cpp \
    nlaudioconfig.cpp \
    nlaudioalsa.cpp \
    nlaudioalsainput.cpp \
    nlaudioalsaoutput.cpp \
    nlrawmididevice.cpp \
    nlaudiofactory.cpp \
    tools.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    nlaudioconfig.h \
    nlaudioalsa.h \
    nlaudioalsainput.h \
    nlaudioalsaoutput.h \
    nlaudio.h \
    nlmidi.h \
    nlrawmididevice.h \
    blockingcircularbuffer.h \
    blockinglinearbuffer.h \
    nlaudiofactory.h \
    tools.h

OTHER_FILES += \
    Todo.txt

