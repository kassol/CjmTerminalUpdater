#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T14:37:28
#
#-------------------------------------------------

QT       += core gui network sql script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CjmTerminalUpdater
TEMPLATE = app


SOURCES += main.cpp databasemanage.cpp updater.cpp updateitem.cpp statusdialog.cpp \
    downloadmanagerHTTP.cpp

HEADERS  += databasemanage.h updater.h updateitem.h statusdialog.h \
    downloadmanagerHTTP.h

FORMS    += statusdialog.ui

RESOURCES += resource.qrc
