#include "Server.h"

#include <NetworkLayer.h>
#include "Packets.h"
#include <iostream>
#include <chrono>
#include <SDL.h>
#include "../Bullet.h"

void Server::Run()
{

    std::vector<Player*> players;
    std::vector<Bullet*> blt;

	EnetServer* s = new EnetServer();
	auto result = s->init("127.0.0.1", "1234", 2);

	if (!result)
	{
		std::cout << "Server could not start (it's probably running already)" << std::endl;
		return;
	}



	Packet data;
	while (1)
	{
//        auto start = std::chrono::high_resolution_clock::now();

		data = s->receive();

        if (data.type != 0)
            printf("[SERVER] Received packet with type %d\n", data.type);
        
        switch (data.type) {
            case PacketType::NEW_PLAYER:
            {
                //This works under the assumption that after a client connects,
                //server receives PLAYER_INFO packet before another client connects
                players.push_back(new Player(NULL, 0, 0));
                players.back()->id = players.size() - 1;
                player_base_info_packet bp = {players.back()->id};
                Packet p = WRAP_PACKET(PacketType::PLAYER_INFO, bp);
                s->send(p, s->clients.back());
                if(players.size() == 2)
                {
                    //start the game if there are 2 players
                    players_positions_packet temp;
                    for(auto p_ : players)
                    {
                        temp.players.push_back(create_player_position_packet(*p_));
                    }
                    Packet p2 = {};
					p2.type = PacketType::PLAYERS_POSITIONS;
                    p2.data = spt::serialize(temp);
                    s->broadcast(p2);
                }
                break;
            }
            case PacketType::PLAYER_POSITION:
            {
                auto temp1 = spt::deserialize<player_position_packet>(data.data);
                handle_player_position_packet(temp1, *players[temp1.id]);
				//start the game if there are 2 players
				players_positions_packet temp;
				for (auto p_ : players)
				{
					temp.players.push_back(create_player_position_packet(*p_));
				}
				Packet p2 = {};
				p2.type = PacketType::PLAYERS_POSITIONS;
				p2.data = spt::serialize(temp);
                //printf("P0 x:%f y:%f\n", temp.players[0].x, temp.players[0].y);
                //printf("P1 x:%f y:%f\n", temp.players[1].x, temp.players[1].y);
				s->broadcast(p2);
                break;
            }
            default: {
                s->broadcast(data);
            }
        }

//        auto end = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double, std::milli> time =
//                std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        //SDL_Delay(50);
	}

	return;
}
