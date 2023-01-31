#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <enet/enet.h>
#include <vector>
#include <shared/packet.h>
#include <stdint.h>
#include <stdio.h>

static std::vector<ENetPeer*> g_remote_peers;

int main(int argc, char* argv[])
{
    if (enet_initialize() != 0) {
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
                printf("Connected to relay %s:%d\n", ip, (int)evt.peer->address.port);

            } else if (evt.type == ENET_EVENT_TYPE_DISCONNECT) {
                printf("Disconnected from relay %s:%d\n", ip, (int)evt.peer->address.port);

                auto it = std::find(g_remote_peers.begin(), g_remote_peers.end(), evt.peer);
                if (it != g_remote_peers.end()) {
                    g_remote_peers.erase(it);
                }

            } else if (evt.type == ENET_EVENT_TYPE_RECEIVE) {

                auto pType = determinePacketType(evt.packet->data);

                switch (pType) {
                case PeerList: {
                    char* p = (char*)evt.packet->data;

                    PeerListPacket packet;
                    packet.deserialize(p);

                    for (auto peer : packet.peers) {

                        char peerListIp[40];

                        ENetAddress addr;
                        addr.host = peer.ip;
                        addr.port = peer.port;

                        enet_address_get_host_ip(&addr, peerListIp, 40);

                        for (ENetPeer* c_peer : g_remote_peers)
                        {
                            // already connected
                            if (c_peer->address.host == addr.host && c_peer->address.port == addr.port)
                                continue;
                        }

                        // don't connect to self
                        if (peer.ip == hostAddress.host && peer.port == hostAddress.port)
                            continue;

                        ENetPeer* peerRemote = enet_host_connect(host, &addr, 2, 0);
                        g_remote_peers.emplace_back(peerRemote);
                    }

                    // disconnect from missing peers

                    for (auto c_peer : g_remote_peers) {
                        for (auto peer : packet.peers) {
                            if (peer.ip == c_peer->address.host && peer.port == c_peer->address.port)
                            {

                            } else {
                            // disconnect peer
                                enet_peer_disconnect(c_peer, 0);
                                g_remote_peers.erase(std::find(g_remote_peers.begin(), g_remote_peers.end(), c_peer));
                            }
                        }
                    }

                    break;
                }
                case NewPeer:
                    break;
                case RemovePeer:
                    break;
                case PlayerPosition:
                    break;
                default:
                    break;
                }
            }

        } 
        else
        {
            printf("... (%d remote peers)\n", (int)g_remote_peers.size());
            for (auto peer : g_remote_peers) {
            }
        }
    }

    enet_deinitialize();
    return 0;
}
