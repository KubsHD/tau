#define _CRT_SECURE_NO_WARNINGS

#include <enet/enet.h>
#include <stdio.h>
#include <vector>
#include <shared/packet.h>

static std::vector<ENetPeer*> g_remote_peers;

int main(int argc, char* argv[])
{
	if (enet_initialize() != 0)
	{
		return EXIT_FAILURE;
	}


	ENetAddress hostAddress;
	hostAddress.host = ENET_HOST_ANY;
	hostAddress.port = ENET_PORT_ANY;
	ENetHost* host = enet_host_create(&hostAddress, 100, 2, 0, 0);

	ENetAddress serverAddress;
	enet_address_set_host(&serverAddress, "127.0.0.1");
	serverAddress.port = 7788;
	ENetPeer* serverPeer = enet_host_connect(host, &serverAddress, 2, 0);

	while (true) {
		ENetEvent evt;
		if (enet_host_service(host, &evt, 1000) > 0) {
			char ip[40];
			enet_address_get_host_ip(&evt.peer->address, ip, 40);

			if (evt.type == ENET_EVENT_TYPE_CONNECT) {
				printf("Connected to %s:%d\n", ip, (int)evt.peer->address.port);

			}
			else if (evt.type == ENET_EVENT_TYPE_DISCONNECT) {
				printf("Disconnected from %s:%d\n", ip, (int)evt.peer->address.port);

				auto it = std::find(g_remote_peers.begin(), g_remote_peers.end(), evt.peer);
				if (it != g_remote_peers.end()) {
					g_remote_peers.erase(it);
				}

			}
			else if (evt.type == ENET_EVENT_TYPE_RECEIVE) {
				char* p = (char*)evt.packet->data;
				Packet pack;
				int cursor = 0;

				PeerCommand com = *(PeerCommand*)(p + cursor);
				cursor += sizeof(PeerCommand);

				pack.peers_count = *(uint32_t*)(p + cursor);
				cursor += sizeof(uint32_t);
				
				for (int i = 0; i < pack.peers_count; i++) {
					Peer peer;
					peer.id = *(uint32_t*)(p + cursor);
					cursor += sizeof(uint32_t);

					peer.ip = *(uint32_t*)(p + cursor);
					cursor += sizeof(uint32_t);

					peer.port = *(uint32_t*)(p + cursor);
					cursor += sizeof(uint32_t);

					ENetAddress addr;
					addr.host = peer.ip;
					addr.port = peer.port;

					char peerListIp[40];
					enet_address_get_host_ip(&addr, peerListIp, 40);

					printf("- %s:%d\n", peerListIp, (int)addr.port);

					ENetPeer* peerRemote = enet_host_connect(host, &addr, 2, 0);
					g_remote_peers.emplace_back(peerRemote);
				}

			}
			else {
				printf("Unknown event from %s\n", ip);
			}
		}
		else {
			printf("... (%d remote peers)\n", (int)g_remote_peers.size());
			for (auto peer : g_remote_peers) {
		
			}
		}
	}

	
    enet_deinitialize();
    return 0;
}
