#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION

#include <SDL.h>
#include <enet/enet.h>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include "lib/stb_image.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define ASSERT_SDL(cond)    if(!(cond)){fprintf(stderr, "SDL broke: %s\n", SDL_GetError());return EXIT_FAILURE;}

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), exit(EXIT_FAILURE))

SDL_Texture* loadTexture(std::string, SDL_Renderer*);

//static std::vector<ENetPeer*> g_remote_peers;

int main(int argc, char* argv[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* t1 = NULL;

    ASSERT_SDL(SDL_Init(SDL_INIT_VIDEO) >= 0)

    //Create window
    window = SDL_CreateWindow("Splatter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    ASSERT_SDL(window != NULL)

    //Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ASSERT_SDL(renderer != NULL)

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    SDL_Texture* burgir = loadTexture("./../../game/data/burgir.png", renderer);

    bool quit = false;
    SDL_Event e;

    while(!quit)
    {
        //Handle events
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        //Clear screen
        SDL_RenderClear(renderer);

        //Render texture
        SDL_RenderCopy(renderer, burgir, NULL, NULL);

        //Update screen
        SDL_RenderPresent(renderer);
    }

    return EXIT_SUCCESS;
}


SDL_Texture* loadTexture(std::string path, SDL_Renderer* renderer)
{
    int w;
    int h;

    int channels;

    const char* asset_path = path.c_str();

    stbi_uc* data = stbi_load(asset_path, &w, &h, &channels, 0);

    delete asset_path;

    if (data == NULL)
    {
        ERR("Couldn't load texture");
    }

    int format = channels == STBI_rgb ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;


    SDL_Texture* tex = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_UpdateTexture(tex, NULL, (const void*)data, w * channels);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    stbi_image_free(data);

    return tex;
}

