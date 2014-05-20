#-------------------------------------------------
#
# Project created by QtCreator 2014-01-25T17:43:37
#
#-------------------------------------------------

QT       -= core gui

TARGET = Assets
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

SOURCES +=

HEADERS +=

include(Model/Model.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Base/release/ -lBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Base/debug/ -lBase
else:unix: LIBS += -L$$OUT_PWD/../Base/ -lBase

INCLUDEPATH += $$PWD/../Base
DEPENDPATH += $$PWD/../Base

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Base/release/libBase.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Base/debug/libBase.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Base/release/Base.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Base/debug/Base.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Base/libBase.a
