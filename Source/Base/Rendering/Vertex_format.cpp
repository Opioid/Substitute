#include "Vertex_format.hpp"
#include "Vertex_layout_description.hpp"

namespace rendering
{

const Vertex_layout_description* Vertex_position2x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32_Float)
	};

	static const Vertex_layout_description description(1, elements);

	return &description;
}

const Vertex_layout_description* Vertex_position3x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float)
	};

	static const Vertex_layout_description description(1, elements);

	return &description;
}

const Vertex_layout_description* Vertex_position3x32_color1x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Color",    0, Data_format::R8G8B8A8_UNorm)
	};

	static const Vertex_layout_description description(2, elements);

	return &description;
}

const Vertex_layout_description* Vertex_position2x32_tex_coord2x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position",  0, Data_format::R32G32_Float),
		Vertex_layout_description::Element("Tex_coord", 0, Data_format::R32G32_Float)
	};

	static const Vertex_layout_description description(2, elements);

	return &description;
}

const Vertex_layout_description* Vertex_position3x32_tex_coord2x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position",  0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Tex_coord", 0, Data_format::R32G32_Float)
	};

	static const Vertex_layout_description description(2, elements);

	return &description;
}

const Vertex_layout_description* Vertex_position3x32_normal3x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Normal",   0, Data_format::R32G32B32_Float)
	};

	static const Vertex_layout_description description(2, elements);

	return &description;
}

/*
const Vertex_layout_description* DistributedVertex_Pos3x32_Tex2x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0, 0),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float, 1, 0)
	};

	static Vertex_layout_description const description(elements, 2);

	return &description;
}


const Vertex_layout_description* InstancedVertex_Rows4x3x32::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Row", 0, Data_format::R32G32B32A32_Float, Vertex_layout_description::Classification::PerInstanceData, 1),
		Vertex_layout_description::Element("Row", 1, Data_format::R32G32B32A32_Float, Vertex_layout_description::Classification::PerInstanceData, 1),
		Vertex_layout_description::Element("Row", 2, Data_format::R32G32B32A32_Float, Vertex_layout_description::Classification::PerInstanceData, 1),
		Vertex_layout_description::Element("Row", 3, Data_format::R32G32B32A32_Float, Vertex_layout_description::Classification::PerInstanceData, 1),
	};

	static Vertex_layout_description const description(elements, 4);

	return &description;
}


const Vertex_layout_description* Vertex_Pos3x32_Norm1010102_Ao1x8::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm),
		Vertex_layout_description::Element("Ao",       0, Data_format::R8_UNorm)
	};

	static Vertex_layout_description const description(elements, 3);

	return &description;
}
*/

const Vertex_layout_description* Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position",  0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Tex_coord", 0, Data_format::R32G32_Float),
		Vertex_layout_description::Element("Normal",    0, Data_format::R10G10B10A2_UNorm)
	};

	static const Vertex_layout_description description(3, elements);

	return &description;
}

/*
const Vertex_layout_description* DistributedVertex_Pos3x32_Tex2x32_Norm1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float, 1),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm, 2)
	};

	static Vertex_layout_description const description(elements, 3);

	return &description;
}


const Vertex_layout_description* Vertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm),
		Vertex_layout_description::Element("Tangent",  0, Data_format::R10G10B10A2_UNorm)
	};

	static Vertex_layout_description const description(elements, 4);

	return &description;
}


const Vertex_layout_description* DistributedVertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float, 1),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm, 2),
		Vertex_layout_description::Element("Tangent",  0, Data_format::R10G10B10A2_UNorm, 2)
	};

	static Vertex_layout_description const description(elements, 4);

	return &description;
}


const Vertex_layout_description* Vertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Weights",  0, Data_format::R8G8B8A8_UNorm),
		Vertex_layout_description::Element("Bones",    0, Data_format::R8G8B8A8_UInt),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm)
	};

	static Vertex_layout_description const description(elements, 5);

	return &description;
}


const Vertex_layout_description* DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0),
		Vertex_layout_description::Element("Weights",  0, Data_format::R8G8B8A8_UNorm, 0),
		Vertex_layout_description::Element("Bones",    0, Data_format::R8G8B8A8_UInt, 0),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float, 1),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm, 2)
	};

	static Vertex_layout_description const description(elements, 5);

	return &description;
}


const Vertex_layout_description* DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Norm1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0),
		Vertex_layout_description::Element("Weights",  0, Data_format::R8G8B8A8_UNorm, 0),
		Vertex_layout_description::Element("Bones",    0, Data_format::R8G8B8A8_UInt, 0),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm, 1)
	};

	static Vertex_layout_description const description(elements, 4);

	return &description;
}


const Vertex_layout_description* Vertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102_Tan1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float),
		Vertex_layout_description::Element("Weights",  0, Data_format::R8G8B8A8_UNorm),
		Vertex_layout_description::Element("Bones",    0, Data_format::R8G8B8A8_UInt),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm),
		Vertex_layout_description::Element("Tangent",  0, Data_format::R10G10B10A2_UNorm)
	};

	static Vertex_layout_description const description(elements, 6);

	return &description;
}


const Vertex_layout_description* DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102_Tan1010102::vertex_layout_description()
{
	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Position", 0, Data_format::R32G32B32_Float, 0),
		Vertex_layout_description::Element("Weights",  0, Data_format::R8G8B8A8_UNorm, 0),
		Vertex_layout_description::Element("Bones",    0, Data_format::R8G8B8A8_UInt, 0),
		Vertex_layout_description::Element("TexCoord", 0, Data_format::R32G32_Float, 1),
		Vertex_layout_description::Element("Normal",   0, Data_format::R10G10B10A2_UNorm, 2),
		Vertex_layout_description::Element("Tangent",  0, Data_format::R10G10B10A2_UNorm, 2)
	};

	static Vertex_layout_description const description(elements, 6);

	return &description;
}
*/
}
