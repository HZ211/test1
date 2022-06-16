TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        tcpserver.cpp \
        thread.cpp

HEADERS += \
    tcpserver.h \
    thread.h

LIBS += -L/usr/local/lib -levent -lpthread
