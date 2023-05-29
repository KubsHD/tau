#include "Player.h"
#include <SDL.h>

void Player::Move(const Uint8 *keyStates) {
    if(keyStates[SDL_SCANCODE_UP])
    {
        rect.y -= speed;
    }
    if(keyStates[SDL_SCANCODE_DOWN])
    {
        rect.y += speed;
    }
    if(keyStates[SDL_SCANCODE_RIGHT])
    {
        rect.x += speed;
    }
    if(keyStates[SDL_SCANCODE_LEFT])
    {
        rect.x -= speed;
    }
}

Player::Player(Texture *texture, int x, int y) : GameObject(texture, x, y)
{

}

Player::Player(Texture *texture, int x, int y, int id) : Player(texture, x, y)
{
    this->id = id;
}
