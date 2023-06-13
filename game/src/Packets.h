#pragma once
#ifndef SPLATTER_PACKETS_H
#define SPLATTER_PACKETS_H

#include <vector>
#include "Serialization.h"
#include "GameObject.h"

class Player;

#define WRAP_PACKET(packet_type, packet)    \
    {                                       \
    packet_type,                            \
    spt::serialize(packet)                  \
    }

enum PacketType {
    PACKET_EMPTY        =   0,
	PLAYER_POSITION     =   1,
	PLAYER_SHOOT        =   2,
    UPDATE_PLAYERS      =   3,
    NEW_PLAYER          =   4,
    PLAYER_INFO         =   5,
    PLAYERS_POSITIONS = 6,
    BULLETS_POSITION_UPDATE
};

namespace spt
{
    template<typename T>
    T deserialize(std::vector<char> vec)
    {
        T packet;
        auto rd = ReadStream32(vec, vec.size());
        packet.Serialize(rd);
        return packet;
    }

    template<typename T>
    std::vector<char> serialize(T packet)
    {
        auto wr = WriteStream32();
        packet.Serialize(wr);
        auto vec = std::vector<char>(wr.GetSize() * 4);
        std::memcpy(vec.data(), wr.GetBuffer(), wr.GetSize() * 4);
        return vec;
    }
}

struct Packet {
    int type;
    std::vector<char> data;
    uint32_t tick;
    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_int32(stream, type);
        serialize_char_vector(stream, data, data.size());
        serialize_uint32(stream, tick);
        return true;
    }
};

struct player_position_packet {
    int id;
    float x;
    float y;

    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_int32(stream, id);
        serialize_float32(stream, x);
        serialize_float32(stream, y);
        return true;
    }
};


struct bullets_position_packet {
    std::vector<int> bullet_idx;
    std::vector<Transform> bullet_positions;
};

void handle_player_position_packet(player_position_packet& packet, Player& player);

player_position_packet create_player_position_packet(const Player& player);

struct player_base_info_packet {
    int id;

    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_uint32(stream, id);
        return true;
    }
};

struct update_players_packet {
    std::vector<player_base_info_packet> players;

    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_vector(stream, players);
        return true;
    }
};

struct players_positions_packet {
    std::vector<player_position_packet> players;

    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_vector2(stream, players);
        return true;
    }
};

struct new_player_packet {
    int dummy_int;

    template<typename Stream> bool Serialize(Stream & stream)
    {
        serialize_int32(stream, dummy_int);
        return true;
    }
};

#endif //SPLATTER_PACKETS_H
