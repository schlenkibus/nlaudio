TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lasound
LIBS += -lpthread

SOURCES += main.cpp \
    nlaudio.cpp \
    nlaudioconfig.cpp \
    nlaudioalsa.cpp \
    nlaudioalsainput.cpp \
    nlaudioalsaoutput.cpp \
    audiobuffer.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    nlaudio.h \
    nlaudioconfig.h \
    nlaudioalsa.h \
    nlaudiointerface.h \
    nlaudioalsainput.h \
    nlaudioalsaoutput.h \
    audiobuffer.h

