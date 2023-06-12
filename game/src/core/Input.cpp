#include "input.h"

#include <SDL.h>
#include <iostream>
#include "Types.h"
#include <cstring>

bool quit;

static Uint8* key_state;
static Uint8* last_key_state;

static SDL_GameController* pad;

static SDL_MouseWheelEvent wheel;

static bool input_locked = false;

static Mouse last_mouse;
static Mouse mouse;



void Input::update(SDL_Event& evt)
{
	std::memcpy(last_key_state, key_state, sizeof(Uint32) * SDL_NUM_SCANCODES);
	std::memcpy(key_state, SDL_GetKeyboardState(NULL), sizeof(Uint32) * SDL_NUM_SCANCODES);

	// KEEP MOUSE TRAPPED INSIDE THE WINDOW
	// SDL_SetRelativeMouseMode(SDL_TRUE);

	last_mouse = mouse;

	mouse.buttons = SDL_GetMouseState(&(mouse.pos.x), &(mouse.pos.y));
	// log_info("[mouse] x: %4d | y: %4d\n", mouse.pos.x, mouse.pos.y);

	if ((mouse.buttons & SDL_BUTTON_LMASK) != 0) {
		mouse.LMB = 1;
		//log_info("Mouse right buttom is pressed");	
	}
	else
		mouse.LMB = 0;

	if ((mouse.buttons & SDL_BUTTON_RMASK) != 0) {
		mouse.RMB = 1;
		//log_info("Mouse left buttom is pressed");
	}
	else
		mouse.RMB = 0;

}

void Input::update_mouse_wheel(SDL_MouseWheelEvent mevt)
{
	wheel = mevt;
}

bool Input::mouse_down(int key)
{
	if (key == 0)
		return mouse.LMB == true && last_mouse.LMB == false;
	else if (key == 1)
		return mouse.RMB == true && last_mouse.RMB == false;

	return false;
}

bool Input::mouse_held(int key)
{
	if (key == 0)
		return mouse.LMB == true;
	else if (key == 1)
		return mouse.RMB == true;

	return false;
}

Vec2 Input::get_mouse_pos()
{
	return mouse.pos;
}


bool Input::key_down(SDL_Scancode scanCode)
{
	return (key_state[scanCode] == 1 && last_key_state[scanCode] == 0);
}

bool Input::key_held(SDL_Scancode scanCode)
{
	return (key_state[scanCode] == 1 && last_key_state[scanCode] == 1);
}


SDL_MouseWheelEvent Input::get_wheel()
{
	return wheel;
}

Input::Input()
{
	
	pad == NULL;

	key_state = (Uint8*)new Uint32[SDL_NUM_SCANCODES]();
	last_key_state = (Uint8*)new Uint32[SDL_NUM_SCANCODES]();
	std::memcpy(key_state, SDL_GetKeyboardState(NULL), sizeof(Uint32) * SDL_NUM_SCANCODES);
	std::memset(last_key_state, 0, sizeof(Uint32) * SDL_NUM_SCANCODES);

}
