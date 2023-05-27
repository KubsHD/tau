#pragma once

struct SDL_Window;

class Window {
public:
	Window(int width, int height, const char* title);
	~Window() {};
	
	SDL_Window* get_ptr();
private:
	SDL_Window* m_ptr;

};
