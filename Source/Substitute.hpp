#pragma once

#ifdef WIN32

#include <windows.h>

#undef min
#undef max

#define SUBSTITUTE_MAIN int CALLBACK WinMain(_In_  HINSTANCE /*hInstance*/, _In_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)

#else

#define SUBSTITUTE_MAIN int main()

#endif
