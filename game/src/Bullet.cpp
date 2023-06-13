#include "Bullet.h"
#include "Player.h"
#include <glm/glm.hpp>

Bullet::Bullet(int mx, int my, Texture* texture, Player* player)
: GameObject(texture, player->GetX(), player->GetY())
{
    float vx_ = (float)mx - transform.x - (float)player->GetWidth() / 2;
    float vy_ = (float)my - transform.y - (float)player->GetHeight() / 2;
    float length = sqrt(vx_ * vx_ + vy_ * vy_);
    vx = speed * vx_ / length;
    vy = speed * vy_ / length;
}

void Bullet::Move()
{
    transform.x += vx;
    transform.y += vy;
    rect.x = (int)transform.x;
    rect.y = (int)transform.y;
}

glm::vec2 Bullet::get_position() {
    return glm::vec2(transform.x, transform.y);
}
