#pragma once

#include <SDL_error.h>
#include <cstdio>
#include <cstdlib>


#define SCREEN_WIDTH 600//1280
#define SCREEN_HEIGHT 400//720

#define ASSERT_SDL(cond)    if(!(cond)){fprintf(stderr, "SDL broke: %s\n", SDL_GetError());abort();}

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), abort())