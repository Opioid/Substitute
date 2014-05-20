#pragma once

#include <cstdint>

namespace platform
{

class Client;

}

namespace rendering
{

class Swapchain
{

public:

	bool init(const platform::Client& client);

	void present() const;

	void set_sync_interval(uint32_t interval);

private:

	const platform::Client* client_;
};

}
