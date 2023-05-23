#ifndef SPLATTER_BULLET_H
#define SPLATTER_BULLET_H

#include "GameObject.h"
#include <glm/fwd.hpp>

class Player;

class Bullet : public GameObject {
private:
    float vx;
    float vy;
    float x;
    float y;
    int speed = 8;
public:
    Bullet(int mx, int my, Texture* texture, Player* player);
    void Move();
    glm::vec2 get_position();
};


#endif //SPLATTER_BULLET_H
