#include "Swapchain.hpp"
#include "../Platform/Client.hpp"
#include "../Platform/Platform.hpp"

namespace rendering
{

bool Swapchain::init(const platform::Client& client)
{
	client_ = &client;

	return true;
}

void Swapchain::present() const
{
	SDL_GL_SwapWindow(client_->handle());
}

void Swapchain::set_sync_interval(uint32_t interval)
{
    SDL_GL_SetSwapInterval(interval);
}

}
