#ifndef SPLATTER_PLAYER_H
#define SPLATTER_PLAYER_H


#include "GameObject.h"

class Player : public GameObject {
private:
    int speed = 3;
public:
    Player(Texture* texture, int x, int y);
    void Move(const Uint8* keyStates);
};


#endif //SPLATTER_PLAYER_H
