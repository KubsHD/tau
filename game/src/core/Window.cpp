#include "Window.h"

#include <SDL3/SDL_video.h>
#include "Macros.h"

Window::Window(int width, int height, const char* title)
{
	m_ptr = SDL_CreateWindow(title, width, height, 0);

	ASSERT_SDL(m_ptr)
}

SDL_Window* Window::get_ptr()
{
	return m_ptr;
}
