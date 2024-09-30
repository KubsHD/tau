#ifndef SPLATTER_PLAYER_H
#define SPLATTER_PLAYER_H


#include "GameObject.h"

class Player : public GameObject {
private:
    int speed = 3;
public:
    int id;
    Player(Texture* texture, int x, int y);
    Player(Texture* texture, int x, int y, int id);
    void Move(const bool* keyStates);
};


#endif //SPLATTER_PLAYER_H
