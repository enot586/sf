QT += core
QT -= gui

TARGET = client
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

HEADERS += ./inc/FileTransmitter.h \
           ./inc/SharedMemoryProtocol.h \
           ./inc/SharedMemoryBuffer.h \
           ./inc/AbstractProtocol.h \
           ./inc/AbstractResource.h\
    client.h

SOURCES += client.cpp \
    ./src/FileTransmitter.cpp \
    ./src/SharedMemoryProtocol.cpp

INCLUDEPATH += ./inc/
INCLUDEPATH += /usr/include/boost
INCLUDEPATH += /usr/include/boost/program_options
INCLUDEPATH += /usr/include/boost/filesytem
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/x86_64-linux-gnu/

LIBS += -lrt
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_filesystem
