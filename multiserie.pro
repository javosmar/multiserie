#-------------------------------------------------
#
# Project created by QtCreator 2017-11-28T09:19:51
#
#-------------------------------------------------

QT       += core gui serialport sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multiserie
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    duthread.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    duthread.h \
    qcustomplot.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=gnu++14

RESOURCES += \
    recurso.qrc
