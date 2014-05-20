#pragma once

namespace rendering
{

class Rendering_device;

class Cache
{

public:

	Cache(const Rendering_device& device);
	virtual ~Cache();

	virtual void release() = 0;

protected:

	const Rendering_device& device_;
};

}


