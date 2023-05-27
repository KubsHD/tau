#pragma once

#include <SDL_error.h>
#include <memory>
#include <cstdio>

namespace spt {

	template<typename T>
	using ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ref<T> create_ref(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr scope<T> create_scope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}


#define SCREEN_WIDTH 600//1280
#define SCREEN_HEIGHT 400//720

#define ASSERT_SDL(cond)    if(!(cond)){fprintf(stderr, "SDL broke: %s\n", SDL_GetError());abort();}

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), abort())