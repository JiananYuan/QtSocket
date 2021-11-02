#-------------------------------------------------
#
# Project created by QtCreator 2019-09-09T09:01:35
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSocket
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_ENV_DEFINE

DEFINES += __WINSOCKET_P__
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib\x64"


CONFIG += c++14

SOURCES += \
    FileManager.cpp \
        main.cpp \
        mainwindow.cpp \
    mchatwidget.cpp \
    socket_service.cpp \
    bindIO.cpp

HEADERS += \
    FileManager.h \
        mainwindow.h \
    socket_service.h \
    mchatwidget.h \
    bindIO.h \
    dataprocessing.h \
    classifier.h

LIBS += -lws2_32
LIBS += -pthread
#win32:INCLUDEPATH += "C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\x86_64-w64-mingw32\include"

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Headers/ -lwavelib
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Headers/ -lwavelib
#else:unix: LIBS += -L$$PWD/Headers/ -lwavelib

#INCLUDEPATH += $$PWD/Headers
#DEPENDPATH += $$PWD/Headers
