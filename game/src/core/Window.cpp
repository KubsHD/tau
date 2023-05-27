#include "Window.h"

#include <SDL_video.h>
#include "Macros.h"

Window::Window(int width, int height, const char* title)
{
	m_ptr = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

	ASSERT_SDL(m_ptr)
}

SDL_Window* Window::get_ptr()
{
	return m_ptr;
}
