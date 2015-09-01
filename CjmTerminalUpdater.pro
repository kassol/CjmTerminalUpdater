#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T14:37:28
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CjmTerminalUpdater
TEMPLATE = app


SOURCES += main.cpp \
    downloadmanagerHTTP.cpp \
    databasemanage.cpp

HEADERS  += \
    downloadmanagerHTTP.h \
    databasemanage.h

FORMS    +=

RESOURCES += \
    resource.qrc
