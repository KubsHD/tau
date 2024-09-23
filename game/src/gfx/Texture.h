#ifndef SPLATTER_TEXTURE_H
#define SPLATTER_TEXTURE_H

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), exit(EXIT_FAILURE))

struct SDL_Texture;
struct SDL_Renderer;

#include <SDL3/SDL_gpu.h>


class Texture {
public:
    SDL_Texture* texture;
    int height;
    int width;
    Texture(const char* path, SDL_Renderer* renderer);
};


#endif //SPLATTER_TEXTURE_H
