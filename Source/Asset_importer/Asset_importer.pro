TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
CONFIG(debug, release|debug):DEFINES += _DEBUG

SOURCES += \
    Main.cpp


include(Export/Export.pri)
include(Import/Import.pri)


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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Libraries/assimp--3.0.1270-sdk/lib/assimp_release-st_x64/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Libraries/assimp--3.0.1270-sdk/lib/assimp_debug-st_x64/ -lassimp
else:unix: LIBS += -lassimp

win32: INCLUDEPATH += $$PWD/../../../../Libraries/assimp--3.0.1270-sdk/include
win32: DEPENDPATH += $$PWD/../../../../Libraries/assimp--3.0.1270-sdk/include

unix: LIBS += -lz
