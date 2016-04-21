#-------------------------------------------------
#
# Project created by QtCreator 2016-04-04T15:26:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14

TARGET = Qms2
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp\
        delegates.cpp

include(Tdc/Tdc.pri)
include(MassSpec/MassSpec.pri)
include(Graphics/Graphics.pri)
include(Deprecated/Deprecated.pri)
include(math/math.pri)

HEADERS  += mainwindow.h\
            delegates.h

FORMS    += mainwindow.ui

RESOURCES += resources.qrc
