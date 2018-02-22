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
    qcustomplot.cpp \
    dialog_nuevo.cpp \
    dbmanager.cpp \
    dialog_hrate.cpp \
    dialognewcourt.cpp

HEADERS  += mainwindow.h \
    duthread.h \
    qcustomplot.h \
    dialog_nuevo.h \
    dbmanager.h \
    dialog_hrate.h \
    dialognewcourt.h

FORMS    += mainwindow.ui \
    dialog_nuevo.ui \
    dialog_hrate.ui \
    dialognewcourt.ui

QMAKE_CXXFLAGS += -std=gnu++14

RESOURCES += \
    recurso.qrc
