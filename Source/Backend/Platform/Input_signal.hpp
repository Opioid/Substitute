#pragma once

#include <cstdint>

namespace platform
{

class Input_signal
{
		
public:

	Input_signal();
	explicit Input_signal(uint32_t id);
	explicit Input_signal(uint32_t id, int data, bool wait_for_release = true);

	uint32_t id() const;

	bool is_character() const;

	bool is_pressed() const;

	int data() const;

	bool wait_for_release() const;

private:

	uint32_t id_;
	bool     is_character_;
	int      data_;
	bool     wait_for_release_;
};

namespace key
{
	extern const uint32_t Grave_accent;

	extern const uint32_t Escape;
	extern const uint32_t Enter;
	extern const uint32_t Tab;
	extern const uint32_t Backspace;
	extern const uint32_t Space;
	extern const uint32_t Delete;

	extern const uint32_t F1;
	extern const uint32_t F2;
	extern const uint32_t F3;
	extern const uint32_t F4;
	extern const uint32_t F5;
	extern const uint32_t F6;
	extern const uint32_t F7;
	extern const uint32_t F8;
	extern const uint32_t F9;
	extern const uint32_t F10;
	extern const uint32_t F11;
	extern const uint32_t F12;

    extern const uint32_t A;
    extern const uint32_t B;
    extern const uint32_t C;
    extern const uint32_t D;
    extern const uint32_t E;
    extern const uint32_t F;
    extern const uint32_t G;
    extern const uint32_t H;
    extern const uint32_t I;
    extern const uint32_t J;
    extern const uint32_t K;
    extern const uint32_t L;
    extern const uint32_t M;
    extern const uint32_t N;
    extern const uint32_t O;
    extern const uint32_t P;
    extern const uint32_t Q;
    extern const uint32_t R;
    extern const uint32_t S;
    extern const uint32_t T;
    extern const uint32_t U;
    extern const uint32_t V;
    extern const uint32_t W;
    extern const uint32_t X;
    extern const uint32_t Y;
    extern const uint32_t Z;

	extern const uint32_t Left_shift;
	extern const uint32_t Left_control;
	extern const uint32_t Left_alt;

	extern const uint32_t Right_shift;
	extern const uint32_t Right_control;
	extern const uint32_t Right_alt;

	extern const uint32_t Up;
	extern const uint32_t Down;
	extern const uint32_t Left;
	extern const uint32_t Right;

    extern const uint32_t Mouse_left;
    extern const uint32_t Mouse_right;
    extern const uint32_t Mouse_middle;

    extern const uint32_t Num_keys;
}

namespace motion
{
    extern const uint32_t Mouse_x;
    extern const uint32_t Mouse_y;
}

}
