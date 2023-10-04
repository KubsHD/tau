#include "GameObject.h"
#include <SDL.h>

GameObject::GameObject(spt::ref<Texture> texture, int x, int y) : texture(texture)
{
    this->transform.x = x;
    this->transform.y = y;

    this->transform.z = 0;

    if(texture == NULL)
        return;
    rect.h = texture->desc.size.y;
    rect.w = texture->desc.size.x;
}

void GameObject::Render()
{
    rect.x = (int)transform.x;
    rect.y = (int)transform.y;
}

SDL_Rect GameObject::GetRect()
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
