SOURCES += \
	Math/Hammersley.cpp \
	Math/Math.cpp \
	Math/Ray.cpp 

HEADERS += \
	Math/half.hpp \ 
	Math/Half_vector.hpp \ 
	Math/Hammersley.hpp \
	Math/Math.hpp \
	Math/Matrix.hpp \
	Math/Matrix.inl \
	Math/Matrix3x3.hpp \
	Math/Matrix3x3.inl \
	Math/Matrix4x3.hpp \
	Math/Matrix4x3.inl \
	Math/Matrix4x4.hpp \
	Math/Matrix4x4.inl \
	Math/Normalized_fixed_point.hpp \
	Math/Normalized_vector.hpp \
	Math/Plane.hpp \
	Math/Quaternion.hpp \
	Math/Quaternion.inl \
	Math/Ray.hpp \
	Math/Simd_intrinsics.hpp \
	Math/Transformation.hpp \
	Math/Vector.hpp \
	Math/Vector.inl \
	Math/Vector2.hpp \
	Math/Vector.inl \
	Math/Vector3.hpp \
	Math/Vector3.inl \
	Math/Vector4.hpp \
	Math/Vector4.inl \

include(Intersection/Intersection.pri)
