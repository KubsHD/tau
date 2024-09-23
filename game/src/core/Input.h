#pragma once
#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <core/types.h>

/// <summary>
/// Struct containing mouse data
/// </summary>
struct Mouse
{
	/// <summary>
	/// Left mouse button state
	/// </summary>
	bool LMB;

	/// <summary>
	/// Right mouse button state
	/// </summary>
	bool RMB;

	/// <summary>
	/// Scroll state
	/// </summary>
	float scroll;

	/// <summary>
	/// Mouse position
	/// </summary>
	glm::vec2 pos;

	Uint32 buttons;
};


/// <summary>
/// Class responsible for input handling through SDL
/// </summary>
class Input
{
public:


	/// <summary>
	/// Initialize input system
	/// </summary>
	Input();

	/// <summary>
	/// Return if a key is down 
	/// </summary>
	/// <param name="scanCode">Key to check</param>
	/// <returns></returns>
	static bool key_down(SDL_Scancode scanCode);
	
	/// <summary>
	/// Return if a key is held
	/// </summary>
	/// <param name="scanCode">key to check</param>
	/// <returns></returns>
	static bool key_held(SDL_Scancode scanCode);

	/// <summary>
	/// Return wheel event
	/// </summary>
	/// <returns></returns>
	static SDL_MouseWheelEvent get_wheel();

	/// <summary>
	/// Update input system
	/// </summary>
	/// <param name="evt">event reference</param>
	void update(SDL_Event& evt);

	/// <summary>
	/// Update mouse wheel
	/// </summary>
	/// <param name="mevt"></param>
	void update_mouse_wheel(SDL_MouseWheelEvent mevt);

	/// <summary>
	/// Check if a mouse button is down
	/// </summary>
	/// <param name="key">mouse btn to check</param>
	/// <returns></returns>
	static bool mouse_down(int key);

	/// <summary>
	/// Check if a mouse button is held
	/// </summary>
	/// <param name="key">mouse btn to check</param>
	/// <returns></returns>
	static bool mouse_held(int key);

	/// <summary>
	/// Get mouse position
	/// </summary>
	/// <returns></returns>
	static Vec2 get_mouse_pos();
};



