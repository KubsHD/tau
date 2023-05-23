#ifndef SPLATTER_GAMEOBJECT_H
#define SPLATTER_GAMEOBJECT_H

#include "Texture.h"
#include "SDL_rect.h"


class GameObject {
public:
    Texture* texture;
    SDL_Rect rect;
    GameObject(Texture* texture, int x, int y);
    void Render(SDL_Renderer* renderer);
};


#endif //SPLATTER_GAMEOBJECT_H
