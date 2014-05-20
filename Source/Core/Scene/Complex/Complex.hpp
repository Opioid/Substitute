#pragma once

#include "../Entity.hpp"

class Resource_manager;

namespace scene
{

class Scene;

class Complex : public Entity
{

public:

	virtual ~Complex();

	virtual bool init(Scene& scene, Resource_manager& resource_manager) = 0;

	virtual void on_tick();
};


class Complex_factory
{
public:

	virtual ~Complex_factory();

	virtual Complex* create_complex() = 0;
};

}
