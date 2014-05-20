#pragma once

#include "OpenGL_object.hpp"
#include "Resources/Handle.hpp"
#include <string>
#include <vector>
#include <map>

namespace rendering
{

class Shader;

class Shader_program : public OpenGL_object
{

public:

	class Signature;

	Shader_program(uint32_t id);
	~Shader_program();

	bool link(std::vector<Handle<Shader>> shaders, std::string& error_message, std::map<std::string, std::string>* semantic_mapping = nullptr);

	void use() const;

	const Signature& get_signature() const;

public:

	class Signature
	{

	public:

		struct Element
		{
			int location;
			std::string semantic;

			bool operator!=(const Element& b) const;
		};

		Signature();
		Signature(const Signature& signature);

		~Signature();

		Signature& operator=(const Signature& signature);
		bool operator==(const Signature& b) const;

		uint32_t get_num_elements() const;
		const Element& operator[](uint32_t index) const;

		void resize(uint32_t num_elements);

	private:

		uint32_t m_num_elements;
		Element* m_elements;

		friend Shader_program;
	};

private:

	Signature m_signature;
};

}


