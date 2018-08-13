#-------------------------------------------------
#
# Project created by QtCreator 2018-05-29T15:05:55
#
#-------------------------------------------------

QT       += core gui network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = L4
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    devserch/dev_search.cpp \
    crc/crc.cpp \
    dock/zdock.cpp

HEADERS  += mainwindow.h \
    devserch/dev_search.h \
    crc/crc.h \
    dock/zdock.h

FORMS    += mainwindow.ui


RESOURCES += \
    resource/l4.qrc

