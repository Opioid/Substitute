#pragma once

#include <map>
#include <string>

class Resource_manager;

namespace scene
{

class Complex;
class Complex_factory;
class Scene;

class Complex_factory_manager
{
public:

	Complex *create_complex(const std::string& type, Scene &scene, Resource_manager& resource_manager);

	void register_factory(Complex_factory* factory, const std::string& type);

private:

	std::map<std::string, Complex_factory*> factories_;
};

}
