#-------------------------------------------------
#
# Project created by QtCreator 2014-06-19T15:07:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtracker
TEMPLATE = app

LIBS += -lopencv_highgui -lopencv_core -lopencv_imgproc


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic
QMAKE_CXXFLAGS_RELEASE += -Ofast -march=native -DNDEBUG
