#ifndef SPLATTER_BULLET_H
#define SPLATTER_BULLET_H

#include "GameObject.h"
#include <glm/fwd.hpp>

class Player;

class Bullet : public GameObject {
private:
    float vx;
    float vy;
    const int speed = 8;



public:
    Bullet(int mx, int my, spt::ref<Texture> texture, Player* player, int player_id, int id);
    void Move();
    glm::vec2 get_position();

    int player_id;
    int id;
};


#endif //SPLATTER_BULLET_H
