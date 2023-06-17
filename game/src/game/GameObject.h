#pragma once

#ifndef SPLATTER_GAMEOBJECT_H
#define SPLATTER_GAMEOBJECT_H

#include <gfx/Texture.h>
#include "SDL_rect.h"
#include <glm/fwd.hpp>


struct Transform {
    float x;
    float y;
    float z; 


};

class GameObject {
protected:
    SDL_Rect rect;
    Transform transform;
public:
    Texture* texture;
    GameObject(Texture* texture, int x, int y);
    void Render(SDL_Renderer* renderer);
    SDL_Rect GetRect();
    float GetX() const;
    float GetY() const;
    void SetX(float value);
    void SetY(float volue);
    int GetHeight() const;
    int GetWidth() const;

    void set_transform(Transform& transform);
    Transform& get_transform();
};


#endif //SPLATTER_GAMEOBJECT_H
