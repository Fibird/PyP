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
    commands/commands.cpp \
    cartoonifier/sketchize.cpp \
    cartoonifier/paintize.cpp \
    cartoonifier/evilize.cpp

HEADERS  += mainwindow.h \
    cartoonifier/sketchize.h \
    commands/commands.h \
    cartoonifier/paintize.h \
    cartoonifier/evilize.h

FORMS    += mainwindow.ui

RESOURCES += \
    cpresources.qrc

TRANSLATIONS += pyp_en.ts \
                pyp_zh.ts
# you need to set a environment called OPENCV_VERSION
# whose value is OPENCV3 or OPENCV2 according to your opencv version
CONFIG += $$(OPENCV_VERSION)

OPENCV3 {

win32 {
message("Using win32 configuration")

# change this variable according to your path of opencv
OPENCV_PATH = D:/opencv3.0/opencv # Note: update with the correct OpenCV version
# change this variable according to your version of opencv
LIBS_PATH = "$$OPENCV_PATH/build/x64/vc14/lib" #project compiled using Visual C++ 2015 64bit compiler

    CONFIG(debug, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_world310d
    }

    CONFIG(release, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_world310
    }
}

}

OPENCV2 {

win32 {
message("Using win32 configuration")

# change this variable according to your path of opencv
OPENCV_PATH = D:/opencv # Note: update with the correct OpenCV version
# change this variable according to your version of opencv
LIBS_PATH = "$$OPENCV_PATH/build/x86/vc12/lib" #project compiled using Visual C++ 2013 32bit compiler

    CONFIG(debug, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core248d \
                -lopencv_imgproc248d \
                -lopencv_highgui248d
    }

    CONFIG(release, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core248 \
                -lopencv_imgproc248 \
                -lopencv_highgui248
    }
}
}
INCLUDEPATH += \
    $$OPENCV_PATH/build/include/

message("OpenCV version: $$(OPENCV_VERSION)")
message("OpenCV path: $$OPENCV_PATH")
message("Includes path: $$INCLUDEPATH")
message("Libraries: $$LIBS")

