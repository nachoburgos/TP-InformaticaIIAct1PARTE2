#-------------------------------------------------
#
# Project created by QtCreator 2012-11-03T13:35:31
#
#-------------------------------------------------

QT       += core gui serialport

TARGET = TP2pro1
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp\
        ventanaprincipal.cpp \
    portcfg.cpp

HEADERS  += ventanaprincipal.h \
    portcfg.h

FORMS    += ventanaprincipal.ui \
    portcfg.ui
