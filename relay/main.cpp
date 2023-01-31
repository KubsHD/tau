#define _CRT_SECURE_NO_WARNINGS

#include <enet/enet.h>
#include <stdio.h>
#include <shared/packet.h>
#include <vector>

static std::vector<ENetPeer*> g_peers;

int main(int argc, char* argv[])
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    ENetAddress hostAddress;
    hostAddress.host = ENET_HOST_ANY;
    hostAddress.port = 7788;
    ENetHost* host = enet_host_create(&hostAddress, 100, 10, 0, 0);

    printf("Listening on port %d\n", (int)hostAddress.port);

    while (true) {
        ENetEvent evt;
        if (enet_host_service(host, &evt, 1000) > 0) {
            char ip[40];
            enet_address_get_host_ip(&evt.peer->address, ip, 40);

            if (evt.type == ENET_EVENT_TYPE_CONNECT) {
                printf("New connection from %s\n", ip);
                g_peers.emplace_back(evt.peer);

                // Send peer list
                PeerListPacket p;
                p.peers_count = g_peers.size();

                for (auto peer : g_peers) {
                    Peer ps;
                    ps.id = 0;
                    ps.ip = peer->address.host;
                    ps.port = peer->address.port;
                    p.peers.push_back(ps);
                }

                printf("Sending PeerListPacket to %d peers\n", p.peers_count);

                auto mem = p.serialize();

                ENetPacket* packet = enet_packet_create(mem,4096, ENET_PACKET_FLAG_RELIABLE);
                enet_host_broadcast(host, 0, packet);


            } else if (evt.type == ENET_EVENT_TYPE_DISCONNECT) {
                printf("Connection closed from %s\n", ip);
                auto it = std::find(g_peers.begin(), g_peers.end(), evt.peer);
                g_peers.erase(it);

            } else if (evt.type == ENET_EVENT_TYPE_RECEIVE) {
                printf("Data received! %d bytes from %s\n", (int)evt.packet->dataLength, ip);

            } else {
                printf("Unknown event from %s\n", ip);
            }
        } else {
            printf("... (%d peers)\n", (int)g_peers.size());
        }

        // input
    }

    enet_deinitialize();
    return 0;
}
