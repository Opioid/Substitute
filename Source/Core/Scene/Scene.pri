SOURCES += \
	Scene/Actor.cpp \
	Scene/Camera.cpp \
	Scene/Entity.cpp \
	Scene/Material.cpp \
	Scene/Material_provider.cpp \
	Scene/Model.cpp \
	Scene/Model_provider.cpp \
	Scene/Node.cpp \
	Scene/Prop.cpp \
	Scene/Renderable.cpp \
	Scene/Scene.cpp \
	Scene/Scene_loader.cpp \
	Scene/Static_prop.cpp \
	Scene/Surface.cpp \
	Scene/Surrounding.cpp \
	Scene/Transformation_interpolator.cpp

HEADERS += \
	Scene/Actor.hpp \
	Scene/Camera.hpp \
	Scene/Entity.hpp \
	Scene/Material.hpp \
	Scene/Material_provider.hpp \
	Scene/Model.hpp \
	Scene/Model_provider.hpp \
	Scene/Node.hpp \
	Scene/Prop.hpp \
	Scene/Renderable.hpp \
	Scene/Scene.hpp \
	Scene/Scene_loader.hpp \
	Scene/Static_prop.hpp \
	Scene/Surface.hpp \
	Scene/Surrounding.hpp \
	Scene/Transformation_interpolator.hpp

include(AABB_tree/AABB_tree.pri)
include(Complex/Complex.pri)
include(Decal/Decal.pri)
include(Editor/Editor.pri)
include(Light/Light.pri)
include(Particles/Particles.pri)
