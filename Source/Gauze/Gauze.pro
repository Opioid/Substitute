#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T12:42:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

TARGET = Gauze
TEMPLATE = app

SOURCES += Main.cpp \

HEADERS += \

FORMS += \


RESOURCES += \
	Resources.qrc

include(Export/Export.pri)
include(GUI/GUI.pri)
include(Image/Image.pri)
include(Image_storage/Image_storage.pri)
include(Settings/Settings.pri)

win32:INCLUDEPATH += .
else:unix:INCLUDEPATH += GUI

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

INCLUDEPATH += $$PWD/../../../../Libraries/rapidjson/include
DEPENDPATH += $$PWD/../../../../Libraries/rapidjson/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvtt/Release_no_cuda/x64/ -lnvtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvtt/Debug_no_cuda/x64/ -lnvtt
else:unix: LIBS += -lnvtt

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvimage/Release/x64/ -lnvimage
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvimage/Debug/x64/ -lnvimage

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvcore/Release/x64/ -lnvcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/nvcore/Debug/x64/ -lnvcore

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/squish/Release/x64/ -lsquish
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/project/vc8/squish/Debug/x64/ -lsquish

INCLUDEPATH += $$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/include
DEPENDPATH += $$PWD/../../../../Libraries/nvidia-texture-tools-2.0.8/include
