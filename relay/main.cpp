#define _CRT_SECURE_NO_WARNINGS

#include <enet/enet.h>
#include <stdio.h>
#include <vector>
#include <shared/packet.h>

static std::vector<ENetPeer*> g_peers;


int main(int argc, char* argv[])
{
	if (enet_initialize() != 0)
	{
		return EXIT_FAILURE;
	}

	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = 7788;
	ENetHost* host = enet_host_create(&hostAddress, 100, 2, 0, 0);

	printf("Listening on port %d\n", (int)hostAddress.port);


	while (true) {
		ENetEvent evt;
		if (enet_host_service(host, &evt, 1000) > 0) {
			char ip[40];
			enet_address_get_host_ip(&evt.peer->address, ip, 40);

			if (evt.type == ENET_EVENT_TYPE_CONNECT) {
				printf("New connection from %s\n", ip);

				// Send peer list
				
				Packet p;
				p.peers_count = g_peers.size();

				for (auto peer : g_peers)
				{
					Peer ps;
					ps.ip = peer->address.host;
					ps.port = peer->address.port;
					p.peers.push_back(ps);
				}

				p.type = pc_PeerList;

				char* mem = (char*)malloc(0x1000);
				int cursor = 0;

				memcpy(mem + cursor, &p.type, sizeof(PeerCommand));
				cursor += sizeof(PeerCommand);
				
				memcpy(mem + cursor, &p.peers_count, sizeof(uint32_t));
				cursor += sizeof(uint32_t);


				for (auto peer : p.peers)
				{
					memcpy(mem + cursor, &peer.id, sizeof(uint32_t));
					cursor += sizeof(uint32_t);

					memcpy(mem + cursor, &peer.ip, sizeof(uint32_t));
					cursor += sizeof(uint32_t);

					memcpy(mem + cursor, &peer.port, sizeof(uint32_t));
					cursor += sizeof(uint32_t);
				}

				ENetPacket* packet = enet_packet_create(mem, cursor, ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(evt.peer, 0, packet);

				g_peers.emplace_back(evt.peer);

			}
			else if (evt.type == ENET_EVENT_TYPE_DISCONNECT) {
				printf("Connection closed from %s\n", ip);
				auto it = std::find(g_peers.begin(), g_peers.end(), evt.peer);
				g_peers.erase(it);

			}
			else if (evt.type == ENET_EVENT_TYPE_RECEIVE) {
				printf("Data received! %d bytes from %s\n", (int)evt.packet->dataLength, ip);

			}
			else {
				printf("Unknown event from %s\n", ip);
			}
		}
		else {
			printf("... (%d peers)\n", (int)g_peers.size());
		}
	}


	enet_deinitialize();
	return 0;
}
