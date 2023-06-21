//
// Created by I3artek on 23/05/2023.
//

#include "Texture.h"
#include "lib/stb_image.h"
#include <SDL.h>

STexture::STexture(const char* path, SDL_Renderer *renderer)
{
    int channels;

    //const char* asset_path = path.c_str();

    stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);

    if (data == NULL)
    {
        ERR("Couldn't load texture");
    }

    int format = channels == STBI_rgb ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;

    texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STATIC, width, height);
    SDL_UpdateTexture(texture, NULL, (const void*)data, width * channels);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    stbi_image_free(data);
}
