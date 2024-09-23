#include "GameObject.h"

GameObject::GameObject(Texture *texture, int x, int y) : texture(texture)
{
    this->transform.x = x;
    this->transform.y = y;
    if(texture == NULL)
        return;
    rect.h = texture->height;
    rect.w = texture->width;
}

void GameObject::Render(SDL_Renderer* renderer)
{
    rect.x = (int)transform.x;
    rect.y = (int)transform.y;
    SDL_RenderTexture(renderer, texture->texture, NULL, &rect);
}

SDL_FRect GameObject::GetRect()
{
    return rect;
}
float GameObject::GetX() const
{
    return transform.x;
}
float GameObject::GetY() const
{
    return transform.y;
}
void GameObject::SetX(float value)
{
    transform.x = value;
}
void GameObject::SetY(float value)
{
    transform.y = value;
}
int GameObject::GetHeight() const
{
    return rect.h;
}
int GameObject::GetWidth() const
{
    return rect.w;
}

void GameObject::set_transform(Transform& transform)
{
    this->transform = transform;
}

Transform& GameObject::get_transform()
{
    return transform;
}
