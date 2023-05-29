#include "GameObject.h"
#include <SDL.h>

GameObject::GameObject(Texture *texture, int x, int y) : texture(texture)
{
    rect.x = x;
    rect.y = y;
    if(texture == NULL)
        return;
    rect.h = texture->height;
    rect.w = texture->width;
}

void GameObject::Render(SDL_Renderer* renderer)
{
    rect.x = (int)x;
    rect.y = (int)y;
    SDL_RenderCopy(renderer, texture->texture, NULL, &rect);
}

SDL_Rect GameObject::GetRect()
{
    return rect;
}
float GameObject::GetX() const
{
    return x;
}
float GameObject::GetY() const
{
    return y;
}
void GameObject::SetX(float value)
{
    x = value;
}
void GameObject::SetY(float value)
{
    y = value;
}
int GameObject::GetHeight() const
{
    return rect.h;
}
int GameObject::GetWidth() const
{
    return rect.w;
}
