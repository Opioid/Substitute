#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T19:51:33
#
#-------------------------------------------------

QT       -= core gui

TARGET = Core
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

DEFINES += CLIP_NEAR_Z_MINUS_ONE

SOURCES +=

HEADERS +=

include(Application/Application.pri)
include(Controls/Controls.pri)
include(File/File.pri)
include(Flags/Flags.pri)
include(Gui/Gui.pri)
include(Logging/Logging.pri)
include(Rendering/Rendering.pri)
include(Resources/Resources.pri)
include(Scene/Scene.pri)
include(Scripting/Scripting.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:INCLUDEPATH += .

INCLUDEPATH += $$PWD/../Base
DEPENDPATH += $$PWD/../Base

INCLUDEPATH += $$PWD/../Backend
DEPENDPATH += $$PWD/../Backend

INCLUDEPATH += $$PWD/../../../../Libraries/rapidjson/include
DEPENDPATH += $$PWD/../../../../Libraries/rapidjson/include

win32:INCLUDEPATH += $$PWD/../../../../Libraries/angelscript_2.28.2/angelscript/include

win32:INCLUDEPATH += $$PWD/../../../../Libraries/boost_1_55_0
win32:DEPENDPATH += $$PWD/../../../../Libraries/boost_1_55_0
