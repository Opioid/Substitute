TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

SOURCES += \
    Main.cpp

include(AABB_tree/AABB_tree.pri)
include(Export/Export.pri)
include(Import/Import.pri)

# Assets ----------------------------------------

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Assets/release/ -lAssets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Assets/debug/ -lAssets
else:unix: LIBS += -L$$OUT_PWD/../Assets/ -lAssets

INCLUDEPATH += $$PWD/../Assets
DEPENDPATH += $$PWD/../Assets

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Assets/release/libAssets.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Assets/debug/libAssets.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Assets/release/Assets.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Assets/debug/Assets.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Assets/libAssets.a

# Base ----------------------------------------

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Substitute-Desktop_Qt_5_3_0_MSVC2013_64bit-Release/Base/release/ -lBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Substitute-Desktop_Qt_5_3_0_MSVC2013_64bit-Debug/Base/debug/ -lBase
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Substitute-Desktop_Clang_64bit-Release/Base/ -lBase
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Substitute-Desktop_Clang_64bit-Debug/Base/ -lBase

INCLUDEPATH += $$PWD/../Base
DEPENDPATH += $$PWD/../Base

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../build-Substitute-Desktop_Qt_5_3_0_MSVC2013_64bit-Release/Base/release/Base.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../build-Substitute-Desktop_Qt_5_3_0_MSVC2013_64bit-Debug/Base/debug/Base.lib
else:unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../build-Substitute-Desktop_Clang_64bit-Release/Base/libBase.a
else:unix:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../build-Substitute-Desktop_Clang_64bit-Debug/Base/libBase.a

INCLUDEPATH += $$PWD/../../../../Libraries/rapidjson/include
DEPENDPATH += $$PWD/../../../../Libraries/rapidjson/include
