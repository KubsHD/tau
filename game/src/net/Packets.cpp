//
// Created by I3artek on 29/05/2023.
//
#include "Packets.h"
#include <game/Player.h>

void handle_player_position_packet(player_position_packet& packet, Player& player)
{
    player.SetX(packet.x);
    player.SetY(packet.y);
}

player_position_packet create_player_position_packet(const Player& player)
{
    Packet packet;
    packet.type = PacketType::PLAYER_POSITION;
    player_position_packet temp = {
            player.id,
            player.GetX(),
            player.GetY()
    };
    packet.data = spt::serialize(temp);
    return temp;
}
