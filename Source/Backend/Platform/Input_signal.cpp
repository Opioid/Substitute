#include "Input_signal.hpp"
#include "Platform.hpp"

namespace platform
{

Input_signal::Input_signal() : id_(0), is_character_(false), data_(0), wait_for_release_(false)
{}

Input_signal::Input_signal(uint32_t id) : id_(id), is_character_(true), data_(0), wait_for_release_(false)
{}

Input_signal::Input_signal(uint32_t id, int data, bool wait_for_release) : id_(id), is_character_(false), data_(data), wait_for_release_(wait_for_release)
{}

uint32_t Input_signal::id() const
{
	return id_;
}

bool Input_signal::is_character() const
{
	return is_character_;
}

bool Input_signal::is_pressed() const
{
	return data_ != 0;
}

int Input_signal::data() const
{
	return data_;
}

bool Input_signal::wait_for_release() const
{
	return wait_for_release_;
}

namespace key
{
    const uint32_t Grave_accent = SDL_SCANCODE_GRAVE;

    const uint32_t Escape    = SDL_SCANCODE_ESCAPE;
    const uint32_t Enter     = SDL_SCANCODE_RETURN;
    const uint32_t Tab       = SDL_SCANCODE_TAB;
    const uint32_t Backspace = SDL_SCANCODE_BACKSPACE;
	const uint32_t Space     = SDL_SCANCODE_SPACE;
    const uint32_t Delete    = SDL_SCANCODE_DELETE;

    const uint32_t F1  = SDL_SCANCODE_F1;
    const uint32_t F2  = SDL_SCANCODE_F2;
    const uint32_t F3  = SDL_SCANCODE_F3;
    const uint32_t F4  = SDL_SCANCODE_F4;
    const uint32_t F5  = SDL_SCANCODE_F5;
    const uint32_t F6  = SDL_SCANCODE_F6;
    const uint32_t F7  = SDL_SCANCODE_F7;
    const uint32_t F8  = SDL_SCANCODE_F8;
    const uint32_t F9  = SDL_SCANCODE_F9;
    const uint32_t F10 = SDL_SCANCODE_F10;
    const uint32_t F11 = SDL_SCANCODE_F11;
    const uint32_t F12 = SDL_SCANCODE_F12;

    const uint32_t A = SDL_SCANCODE_A;
    const uint32_t B = SDL_SCANCODE_B;
    const uint32_t C = SDL_SCANCODE_C;
    const uint32_t D = SDL_SCANCODE_D;
    const uint32_t E = SDL_SCANCODE_E;
    const uint32_t F = SDL_SCANCODE_F;
    const uint32_t G = SDL_SCANCODE_G;
    const uint32_t H = SDL_SCANCODE_H;
    const uint32_t I = SDL_SCANCODE_I;
    const uint32_t J = SDL_SCANCODE_J;
    const uint32_t K = SDL_SCANCODE_K;
    const uint32_t L = SDL_SCANCODE_L;
    const uint32_t M = SDL_SCANCODE_M;
    const uint32_t N = SDL_SCANCODE_N;
    const uint32_t O = SDL_SCANCODE_O;
    const uint32_t P = SDL_SCANCODE_P;
    const uint32_t Q = SDL_SCANCODE_Q;
    const uint32_t R = SDL_SCANCODE_R;
    const uint32_t S = SDL_SCANCODE_S;
    const uint32_t T = SDL_SCANCODE_T;
    const uint32_t U = SDL_SCANCODE_U;
    const uint32_t V = SDL_SCANCODE_V;
    const uint32_t W = SDL_SCANCODE_W;
    const uint32_t X = SDL_SCANCODE_X;
    const uint32_t Y = SDL_SCANCODE_Y;
    const uint32_t Z = SDL_SCANCODE_Z;

    const uint32_t Left_shift   = SDL_SCANCODE_LSHIFT;
    const uint32_t Left_control = SDL_SCANCODE_LCTRL;
    const uint32_t Left_alt     = SDL_SCANCODE_LALT;

    const uint32_t Right_shift   = SDL_SCANCODE_RSHIFT;
    const uint32_t Right_control = SDL_SCANCODE_RCTRL;
    const uint32_t Right_alt     = SDL_SCANCODE_RALT;

    const uint32_t Up    = SDL_SCANCODE_UP;
    const uint32_t Down  = SDL_SCANCODE_DOWN;
    const uint32_t Left  = SDL_SCANCODE_LEFT;
    const uint32_t Right = SDL_SCANCODE_RIGHT;

	const uint32_t Mouse_left   = SDL_NUM_SCANCODES + 1;
	const uint32_t Mouse_right  = Mouse_left + 1;
	const uint32_t Mouse_middle = Mouse_right + 1;

	const uint32_t Num_keys = Mouse_middle + 1;
}

namespace motion
{
    const uint32_t Mouse_x = key::Num_keys;
    const uint32_t Mouse_y = Mouse_x + 1;
}

}
