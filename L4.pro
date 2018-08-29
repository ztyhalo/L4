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
    dock/zdock.cpp \
    zedit/ZEdit.cpp \
    zedit/ZGroup.cpp \
    zedit/ZInternetSet.cpp \
    devserch/dev_client.cpp

HEADERS  += mainwindow.h \
    devserch/dev_search.h \
    crc/crc.h \
    dock/zdock.h \
    zedit/ZEdit.h \
    zedit/ZGroup.h \
    zedit/ZInternetSet.h \
    devserch/dev_client.h

FORMS    += mainwindow.ui


RESOURCES += \
    resource/l4.qrc

