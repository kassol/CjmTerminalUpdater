#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T14:37:28
#
#-------------------------------------------------

QT       += core gui network sql script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CjmTerminalUpdater
TEMPLATE = app


SOURCES += main.cpp downloadmanagerHTTP.cpp databasemanage.cpp updater.cpp updateitem.cpp statusdialog.cpp

HEADERS  += downloadmanagerHTTP.h databasemanage.h updater.h updateitem.h statusdialog.h

FORMS    += statusdialog.ui

RESOURCES += resource.qrc
