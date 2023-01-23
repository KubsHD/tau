#pragma once 

enum PeerCommand : uint8_t
{
	pc_None,
	pc_PeerList,
	pc_NewPeer,
};

struct Peer {
	uint32_t id;
	uint32_t ip;
	uint32_t port;
};

struct Packet {
	PeerCommand type;
	uint32_t peers_count;
	std::vector<Peer> peers;
};
