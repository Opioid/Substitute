#pragma once


#include "Triangle.hpp"
#include <vector>


struct Plane;


bool triangle_behind_plane(const Triangle &tri, const Plane &plane);
bool triangle_intersect_plane(const Triangle &tri, const Plane &plane);

//bool clipTriangle(std::vector<Triangle> &result, const Triangle &tri, const Plane &plane);


bool clipTriangle(std::vector<Triangle> &result, const Triangle &tri, const Plane &plane, std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> &positive, std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> &negative);
