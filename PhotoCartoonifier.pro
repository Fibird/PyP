#-------------------------------------------------
#
# Project created by QtCreator 2017-04-14T16:30:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoCartoonifier
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    cartoon.cpp \
    imageutils.cpp

HEADERS  += mainwindow.h \
    cartoon.h \
    imageutils.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -LD:/opencv3.0/opencv/build/x64/vc14/lib/ -lopencv_world310
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/opencv3.0/opencv/build/x64/vc14/lib/ -lopencv_world310d
else:unix: LIBS += -LD:/opencv3.0/opencv/build/x64/vc14/lib/ -lopencv_world310

INCLUDEPATH += D:/opencv3.0/opencv/build/include
DEPENDPATH += D:/opencv3.0/opencv/build/include
