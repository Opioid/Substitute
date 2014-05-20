SOURCES += Rendering/Scene_rendering/Deferred_scene_renderer.cpp \
Rendering/Scene_rendering/Main_scene_renderer.cpp \
Rendering/Scene_rendering/Scene_renderer.cpp \
Rendering/Scene_rendering/Surface_collector.cpp \
Rendering/Scene_rendering/Test_scene_renderer0.cpp 

HEADERS += Rendering/Scene_rendering/Deferred_scene_renderer.hpp \
Rendering/Scene_rendering/Main_scene_renderer.hpp \
Rendering/Scene_rendering/Scene_renderer.hpp \
Rendering/Scene_rendering/Surface_collector.hpp \
Rendering/Scene_rendering/Scene_renderer.hpp

include(Deferred_volume/Deferred_volume.pri)
include(Help_visualization/Help_visualization.pri)
include(Lighting/Lighting.pri)
include(Shadowing/Shadowing.pri)
include(Surrounding/Surrounding.pri)
