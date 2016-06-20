#-------------------------------------------------
#
# Project created by QtCreator 2016-04-13T10:56:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PAS-ANNOTATION-TOOL
TEMPLATE = app


SOURCES += \
    src/aboutdialog.cpp \
    src/bbox.cpp \
    src/filesupport.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/tourialdialog.cpp \
    src/wlabel.cpp

HEADERS  += \
    src/aboutdialog.h \
    src/bbox.h \
    src/filesupport.h \
    src/mainwindow.h \
    src/tourialdialog.h \
    src/wlabel.h

FORMS    += \
    ui/aboutdialog.ui \
    ui/mainwindow.ui \
    ui/tourialdialog.ui

RESOURCES += \
    ui/resource.qrc

RC_FILE = ui/logo.rc

