#include "Complex_factory_manager.hpp"
#include "Complex.hpp"

namespace scene
{

Complex* Complex_factory_manager::create_complex(const std::string& type, Scene& scene, Resource_manager& resource_manager)
{
	auto factory = m_factories.find(type);

	if (m_factories.end() != factory)
	{
		Complex* complex = factory->second->create_complex();

		if (!complex->init(scene, resource_manager))
		{
			delete complex;
			return nullptr;
		}

		return complex;
	}

	return nullptr;
}

void Complex_factory_manager::register_factory(Complex_factory* factory, const std::string& type)
{
	m_factories[type] = factory;
}

}
