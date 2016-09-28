TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    exceptions.cpp \
    parcer.cpp \
    routingtablessolver.cpp

HEADERS += \
    exceptions.h \
    types.h \
    parcer.h \
    routingtablessolver.h

DISTFILES += \
    readme.txt
