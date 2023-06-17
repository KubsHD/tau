#include "Server.h"

#include <net/NetworkLayer.h>
#include <net/Packets.h>
#include <iostream>
#include <chrono>
#include <SDL.h>
#include <game/Bullet.h>

struct PlayerData {
	int id;
	std::vector<char> avatar_texture_name;
};

void Server::Run()
{

	std::vector<PlayerData> players;
	std::vector<Bullet*> blt;

	EnetServer* s = new EnetServer();
	auto result = s->init("127.0.0.1", "1234", 8);

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
			auto new_player = spt::deserialize<new_player_packet>(data.data);


			int new_player_id = players.size();
			players.push_back(PlayerData{
					.id = new_player_id,
					.avatar_texture_name = new_player.avatar_texture_name
				});

			// assign the newly created player id to packet
			new_player.pid = new_player_id;

			// send new player his id
			s->send(WRAP_PACKET(PacketType::CLIENT_RECV_ID, client_recv_id_packet(new_player_id)), s->sender);

			// request all already connected clients to spawn new player
			for (auto peer : s->clients)
			{
				if (peer != s->sender)
				{
					s->send(WRAP_PACKET(PacketType::PLAYER_SPAWN, new_player), peer);
				}
			}

			// send the new player already connected clients
			for (auto player: players)
			{
				if (player.id != new_player_id)
				{
					new_player.avatar_texture_name = player.avatar_texture_name;
					new_player.pid = player.id;

					s->send(WRAP_PACKET(PacketType::PLAYER_SPAWN, new_player), s->sender);
				}
			}

			break;

			//This works under the assumption that after a client connects,
			//server receives PLAYER_INFO packet before another client connects
			//players.back()->id = players.size() - 1;
			//player_base_info_packet bp = { players.back()->id };
			//Packet p = WRAP_PACKET(PacketType::PLAYER_INFO, bp);
			//s->send(p, s->clients.back());
			//if (players.size() == 2)
			//{
			//	//start the game if there are 2 players
			//	players_positions_packet temp;
			//	for (auto p_ : players)
			//	{
			//		temp.players.push_back(create_player_position_packet(*p_));
			//	}
			//	Packet p2 = {};
			//	p2.type = PacketType::PLAYERS_POSITIONS;
			//	p2.data = spt::serialize(temp);
			//	s->broadcast(p2);
			//}
			//break;
		}
		//case PacketType:
		//{
		//	return;
		//	auto temp1 = spt::deserialize<player_position_packet>(data.data);
		//	handle_player_position_packet(temp1, *players[temp1.id]);
		//	//start the game if there are 2 players
		//	players_positions_packet temp;
		//	for (auto p_ : players)
		//	{
		//		temp.players.push_back(create_player_position_packet(*p_));
		//	}
		//	Packet p2 = {};
		//	p2.type = PacketType::PLAYERS_POSITIONS;
		//	p2.data = spt::serialize(temp);
		//	//printf("P0 x:%f y:%f\n", temp.players[0].x, temp.players[0].y);
		//	//printf("P1 x:%f y:%f\n", temp.players[1].x, temp.players[1].y);
		//	s->broadcast(p2);
		//	break;
		//}
		default: {
			s->broadcast_except_sender(data);
		}
		}

		//        auto end = std::chrono::high_resolution_clock::now();
		//        std::chrono::duration<double, std::milli> time =
		//                std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

				//SDL_Delay(50);
	}

	return;
}
