#pragma once
#ifndef SPLATTER_PACKETS_H
#define SPLATTER_PACKETS_H

#include <vector>
#include <iterator>
#include "../utils/Serialization.h"
#include "../game/GameObject.h"
#include <string>

class Player;

#define WRAP_PACKET(packet_type, packet)    \
    {                                       \
    packet_type,                            \
    spt::serialize(packet)                  \
    }

#define SEND_PACKET(client, packet_type, packet)    \
    client->send({                                       \
    packet_type,                            \
    spt::serialize(packet)                  \
    });                                     \

enum PacketType {
    PACKET_EMPTY        =   0,
	PLAYER_POSITION     =   1,
	PLAYER_SHOOT        =   2,
    UPDATE_PLAYERS      =   3,
    NEW_PLAYER          =   4,
    PLAYER_INFO         =   5,
    PLAYERS_POSITIONS   = 6,
    BULLET_POSITION_UPDATE = 7,
    PLAYER_SPAWN = 8,
    CLIENT_RECV_ID = 9,
	MAX_PACKET_TYPE
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
        serialize_char_vector(stream, data);
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

struct player_positions_packet {
	std::vector<player_position_packet> players;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		serialize_vector2(stream, players);
		return true;
	}
};

struct bullet_postiion_packet {
	std::vector<uint32_t> bullet_position;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		serialize_vector2(stream, bullet_position);
		return true;
	}
};

struct new_player_packet {

    new_player_packet()
    {

    }

    new_player_packet(std::string avatar_texture_name_str)
    {
        pid = -1;
        std::copy(avatar_texture_name_str.begin(), avatar_texture_name_str.end(), std::back_inserter(avatar_texture_name));
    }

    int pid;
	std::vector<char> avatar_texture_name;

    template<typename Stream> bool Serialize(Stream& stream)
    {
        serialize_int32(stream, pid);
        serialize_char_vector(stream, avatar_texture_name);
        return true;
    }
};

struct client_recv_id_packet {

    client_recv_id_packet()
    {

    }

	client_recv_id_packet(int id) : pid(id)
	{

	}

	int pid;

	template<typename Stream> bool Serialize(Stream& stream)
	{
		serialize_int32(stream, pid);
		return true;
	}
};

#endif //SPLATTER_PACKETS_H
