#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T19:51:17
#
#-------------------------------------------------

QT       -= core gui

TARGET = Backend
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

DEFINES += CLIP_NEAR_Z_MINUS_ONE

win32:DEFINES += GLEW_STATIC

SOURCES +=

HEADERS +=

include(File/File.pri)
include(Fonts/Fonts.pri)
include(Platform/Platform.pri)
include(Rendering/Rendering.pri)
include(Scripting/Scripting.pri)
include(Timing/Timing.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:INCLUDEPATH += .

INCLUDEPATH += $$PWD/../Base
DEPENDPATH += $$PWD/../Base

win32:INCLUDEPATH += $$PWD/../../../../Libraries/glew-1.10.0/include

win32:INCLUDEPATH += $$PWD/../../../../Libraries/FreeImage-3.16.0/Source

win32:INCLUDEPATH += $$PWD/../../../../Libraries/physfs-2.0.3

win32:INCLUDEPATH += $$PWD/../../../../Libraries/SDL2-2.0.3/include

win32:INCLUDEPATH += $$PWD/../../../../Libraries/freetype-2.5.3/include
else:unix:INCLUDEPATH += $$PWD/../../../../../../../usr/local/include/freetype2
unix:DEPENDPATH += $$PWD/../../../../../../../usr/local/include/freetype2

INCLUDEPATH += $$PWD/../../../../Libraries/gli-0.5.1.1
DEPENDPATH += $$PWD/../../../../Libraries/gli-0.5.1.1

INCLUDEPATH += $$PWD/../../../../Libraries/gli-0.5.1.1/external/glm-0.9.5.1
DEPENDPATH += $$PWD/../../../../Libraries/gli-0.5.1.1/external/glm-0.9.5.1

win32:INCLUDEPATH += $$PWD/../../../../Libraries/angelscript_2.28.2/angelscript/include

INCLUDEPATH += $$PWD/../../../../Libraries/rapidjson/include
DEPENDPATH += $$PWD/../../../../Libraries/rapidjson/include
