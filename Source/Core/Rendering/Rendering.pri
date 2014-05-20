SOURCES += Rendering/Constant_buffer_cache.cpp \
	Rendering/Printer.cpp \
	Rendering/Renderer.cpp \
	Rendering/Rendering_context.cpp \
	Rendering/Rendering_object.cpp \
	Rendering/Rendering_tool.cpp \
	Rendering/Texture_provider.cpp

HEADERS += Rendering/Constant_buffer_cache.hpp \
	Rendering/Printer.hpp \
	Rendering/Renderer.hpp \
	Rendering/Rendering_context.hpp \
	Rendering/Rendering_object.hpp \
	Rendering/Rendering_tool.hpp \
	Rendering/Texture_provider.hpp

include(Baking/Baking.pri)
include(Caches/Caches.pri)
include(Effect/Effect.pri)
include(Post_processing/Post_processing.pri)
include(Scene_rendering/Scene_rendering.pri)
