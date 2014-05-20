#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T19:51:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = Base
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

DEFINES += CLIP_NEAR_Z_MINUS_ONE

SOURCES +=

HEADERS +=

include(File/File.pri)
include(Math/Math.pri)
include(Parsing/Parsing.pri)
include(Rendering/Rendering.pri)
include(Resources/Resources.pri)
include(String/String.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:INCLUDEPATH += .

win32:INCLUDEPATH += $$PWD/../../../../Libraries/boost_1_55_0

INCLUDEPATH += $$PWD/../../../../Libraries/rapidjson/include
DEPENDPATH += $$PWD/../../../../Libraries/rapidjson/include
