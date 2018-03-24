[![Project Status: WIP - Initial development is in progress, but there has not yet been a stable, usable release suitable for the public.](http://www.repostatus.org/badges/latest/wip.svg)](http://www.repostatus.org/#wip)

# Overview

![logo](http://ogx55zj9n.bkt.clouddn.com/pyp_logo.png)

PyP /paip/ is a tiny photo editor. The name "PyP" is a acronym for "Paint your Photos". Just like its name goes, you can "paint" your photos by this software. This software is developed by Qt using OpenCV library.

# Installation

Only one type of application is available - Windows desktop in current, but we'll develop other types later. You can download it [here](http://files.git.oschina.net/group1/M00/01/5A/PaAvDFk5SKSALBZ6AJpnMbpiwhM489.exe?token=9bdb41d0004d240d6a41237c394ed490&ts=1512270462&attname=Setup.exe).

# Features

## Pencil drawing effect

![sketch](http://oqny23dsv.bkt.clouddn.com/sketch-demo.png)

You can get a pencil draw from your photo easily and fast, only one click. And you can also select different styles according to your preference.

## Cartoon effect

![cartoon](http://oqny23dsv.bkt.clouddn.com/cartoon-demo.png)

Make your own cartoon characters using your personal photos.

## Villain effect

![villain](http://oqny23dsv.bkt.clouddn.com/evil-demo.png)

Create a villain character using your photos.

## Avatars making

Developing...

## Coloring photos manually

Developing...

# Known bugs

- [ ] [undo-redo problem](https://github.com/Fibird/PyP/issues/29)
- [ ] [software migration problem](https://github.com/Fibird/PyP/issues/12)


# Contributing

## Environment

If you want to make Contribution on this project, you need to do this:

- Install OpenCV

You can install OpenCV 2.x.x or OpenCV 3.x.x according to whatever you want. But I suggest you installing OpenCV below 3.0.0, otherwise your application will be very large.

- Install Visual Studio

This software is compiled by MSVC compiler, so you need to install Visual Studio. Note that you need to make your OpenCV complatible with your Visual Studio's version.

- Install Qt Tool Kit

You should install Qt according to your version of Visual Studio.

- Install MSVC Debugger in Qt


- Configure

First, you should add an environment variable called `OPENCV_VERSION` which is either OPENCV2 or OPENCV3. And then you need to modify Qt project file called `PyP.pro` which is in the root directory of this project. Find this part in the file as follows:

```
OPENCV3 {
    win32 {
        message("Using win32 configuration")

        # change this variable according to your path of opencv
        OPENCV_PATH = D:/opencv # Note: update with the correct OpenCV version
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
        LIBS_PATH = "$$OPENCV_PATH/build/x64/vc12/lib"

        CONFIG(debug, debug|release) {
            LIBS     += -L$$LIBS_PATH \
                        -lopencv_core249d \
                        -lopencv_imgproc249d \
                        -lopencv_highgui249d
           }

        CONFIG(release, debug|release) {
            LIBS     += -L$$LIBS_PATH \
                        -lopencv_core249 \
                        -lopencv_imgproc249 \
                        -lopencv_highgui249
           }
    }
}
```

If you using OpenCV2, just modify the OPENCV2 part according to your environment including the **path** of opencv and **name** of OpenCV dynamic link library like opencv_core249. For example, if you use OpenCV 2.4.12, you need to replace the suffix 249 in the name of all libs like -lopencv_core249d with 2412.

## Language

You need to generate language files using Qt Linguist before running it and put them into folder named *languages* which is in the same path of executable program.

# License

![gpl-license](https://www.gnu.org/graphics/gplv3-127x51.png)

Software License Agreement (GPL License)
A tiny photo editor for painting your photos.
Copyright (c) 2017, Chaoyang Liu

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
