#-------------------------------------------------
#
# Project created by QtCreator 2015-07-07T11:22:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-gui-test
TEMPLATE = app

LIBS += -L/usr/local/lib -lgit2


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
