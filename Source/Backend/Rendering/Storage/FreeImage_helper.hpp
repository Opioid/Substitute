#pragma once

#include <FreeImage.h>
#include <string>

namespace fi
{

void error_handler(FREE_IMAGE_FORMAT fif, const char* message);
const std::string& error_message();

unsigned int read(void* buffer, unsigned int size, unsigned int count, fi_handle handle);
int write(void *buffer, unsigned size, unsigned count, fi_handle handle);
int seek(fi_handle handle, long offset, int origin);
long tell(fi_handle handle);

}


