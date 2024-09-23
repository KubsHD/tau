#include "Player.h"
#include <SDL3/SDL.h>

void Player::Move(const Uint8 *keyStates) {
    if(keyStates[SDL_SCANCODE_UP])
    {
        transform.y -= speed;
    }
    if(keyStates[SDL_SCANCODE_DOWN])
    {
        transform.y += speed;
    }
    if(keyStates[SDL_SCANCODE_RIGHT])
    {
        transform.x += speed;
    }
    if(keyStates[SDL_SCANCODE_LEFT])
    {
        transform.x -= speed;
    }
}

Player::Player(Texture *texture, int x, int y) : GameObject(texture, x, y)
{

}

Player::Player(Texture *texture, int x, int y, int id) : Player(texture, x, y)
{
    this->id = id;
}
