#include "Texture_storage.hpp"
#include <FreeImage.h>

namespace rendering
{

namespace texture_storage
{

bool init()
{
	FreeImage_Initialise();

//	FreeImage_SetOutputMessage(fi_error_handler);

	return true;
}

void release()
{
	FreeImage_DeInitialise();
}

}

}
