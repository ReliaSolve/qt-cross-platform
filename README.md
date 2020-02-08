# qt-opengl-cross-platform

This project forms a starting example for building an OpenGL/GLES based program
that compiles and runs on Windows, Mac, Linux, Android, and iOS.

The initial commit was tested on Feb 8, 2020 on the following configurations:
* Windows 10 Pro Feb 2020 release
* Ubuntu 18.04.03
* macOS 10.15.1
* iPhone simulator 11.3.1
* Android 10 kernel 4.9.186 (Tue Jan 14, 2020)

# Building

The project can be opened with QtCreator and configured with the appropriate kit.
It can also be built from the command line on Linux using qmake.

# Reusing

There are @todo comments in the code where changes should be made to the project
to change the behavior to what is desired.  Note that all changes should conform to
both OpenGL 3.3 and GLES 3.0 (for example, glClearDepthf() should be used on GLES
whereas glClearDepth() should be used on Opengl), with QOpenGLContext::currentContext()->isOpenGLES()
being used to determine whether the context is GLES.

This code is licensed under the BSD-3-clause license (see LICENSE.txt), but of course it
requires a separate license for Qt to build.

# Resources

The following links provide information helpful to those developing cross-platform
programs using OpenGL/ES with Qt version 5.

* [OpenGL Window example] (https://doc.qt.io/qt-5/qtgui-openglwindow-example.html)
* [QOpenGLWindow class] (https://doc.qt.io/qt-5/qopenglwindow.html#details)
* [Cross-platform OpenGL/ES](https://www.qt.io/blog/2015/09/09/cross-platform-opengl-es-3-apps-with-qt-5-6)
* [Building Qt for Android](https://wiki.qt.io/Android)

