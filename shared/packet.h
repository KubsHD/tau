#pragma once

#include <stdint.h>
#include <vector>

enum PacketType : uint8_t {
    PeerList,
    NewPeer,
    RemovePeer,

    PlayerPosition
};

PacketType determinePacketType(byte* data)
{
    PacketType type;
    memcpy(&type, data, sizeof(PacketType));
    return type;
}

struct Peer {
    Peer() {};

    uint32_t id;
    uint32_t ip;
    uint32_t port;
};

struct Header {
    PacketType type;

    Header()
        : type() {};

     Header(PacketType t)
        : type(t) {};
};

struct PeerListPacket {
    PacketType type = PacketType::PeerList;
    uint32_t peers_count;
    std::vector<Peer> peers;

    char* serialize()
    {
        char* data = new char[0x1000];
        int cursor = 0;

        memcpy(data + cursor, &type, sizeof(PacketType));
        cursor += sizeof(PacketType);

        memcpy(data + cursor, &peers_count, sizeof(uint32_t));
        cursor += sizeof(uint32_t);

        for (auto peer : peers) {
            memcpy(data + cursor, &peer.id, sizeof(uint32_t));
            cursor += sizeof(uint32_t);

            memcpy(data + cursor, &peer.ip, sizeof(uint32_t));
            cursor += sizeof(uint32_t);

            memcpy(data + cursor, &peer.port, sizeof(uint32_t));
            cursor += sizeof(uint32_t);
        }

        return data;
    }
    
    void deserialize(char* data)
    {
        int cursor = 0;

        memcpy(&type, data + cursor, sizeof(PacketType));
        cursor += sizeof(PacketType);

        memcpy(&peers_count, data + cursor, sizeof(uint32_t));
        cursor += sizeof(uint32_t);

        for (int i = 0; i < peers_count; i++) {

            Peer peer;

            memcpy(&peer.id, data + cursor, sizeof(uint32_t));
            cursor += sizeof(uint32_t);
            
            memcpy(&peer.ip, data + cursor, sizeof(uint32_t));
            cursor += sizeof(uint32_t);
            
            memcpy(&peer.port, data + cursor, sizeof(uint32_t));
            cursor += sizeof(uint32_t);

            peers.push_back(peer);
        }
        
    }
};

struct NewPeerPacket {
    PacketType type = PacketType::NewPeer;
    Peer new_peer;
};

struct PositionPacket {
    PacketType type = PacketType::NewPeer;
    Peer new_peer;
};
