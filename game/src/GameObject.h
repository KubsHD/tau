#ifndef SPLATTER_GAMEOBJECT_H
#define SPLATTER_GAMEOBJECT_H

#include "Texture.h"
#include "SDL_rect.h"


class GameObject {
protected:
    SDL_Rect rect;
    float x;
    float y;
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
};


#endif //SPLATTER_GAMEOBJECT_H
