QT += core
QT -= gui

TARGET = server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += /usr/include/boost
INCLUDEPATH += /usr/include/boost/program_options
INCLUDEPATH += /usr/include/boost/filesytem
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/x86_64-linux-gnu/
INCLUDEPATH += ./inc/

HEADERS += ./inc/FileTransmitter.h \
           ./inc/SharedMemoryProtocol.h \

LIBS += -lrt
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_filesystem

SOURCES += server.cpp \
    ./src/FileTransmitter.cpp \
    ./src/SharedMemoryProtocol.cpp \


