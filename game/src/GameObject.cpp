#include "GameObject.h"
#include <SDL.h>

GameObject::GameObject(Texture *texture, int x, int y) : texture(texture)
{
    rect.x = x;
    rect.y = y;
    rect.h = texture->height;
    rect.w = texture->width;
}

void GameObject::Render(SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, texture->texture, NULL, &rect);
}
