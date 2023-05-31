#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION

#include <SDL.h>
#include <enet/enet.h>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include "lib/stb_image.h"
#include "Texture.h"
#include "Player.h"
#include "Bullet.h"
#include "NetworkLayer.h"
#include <glm/glm.hpp>
#include <memory>
#include <algorithm>
#include <enet/enet.h>
#include <thread>
#include <iostream>
#include <server/Server.h>
#include <Packets.h>
#include <sstream>

#include <core/Window.h>


#include <lib/cereal/types/unordered_map.hpp>
#include <lib/cereal/types/memory.hpp>
#include <lib/cereal/archives/binary.hpp>
#include <lib/cereal/archives/portable_binary.hpp>
#include <core/Macros.h>

SDL_Texture* loadTexture(std::string, SDL_Renderer*);

//static std::vector<ENetPeer*> g_remote_peers;

const char* get_real_path(const char* vpath)
{
#if DEBUG
#if APPLE
    const char* path_prefix = "./../../game/data/";
#else
    const char* path_prefix = "../../../../game/data/";
#endif

#else
#if VITA
    path_prefix = "app0:data/";
#elif XBOX
    char* base = SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_INSTALLED_LOCATION);
    int size = snprintf(NULL, 0, "%s/data/", base);
    char* buf = malloc(size + 1);
    sprintf(buf, "%s/data/", base);
    path_prefix = buf;
#elif APPLE

    char* base = SDL_GetBasePath();
    int size = snprintf(NULL, 0, "%sdata/", base);
    char* buf = (char*)malloc(size + 1);
    sprintf(buf, "%sdata/", base);
    path_prefix = buf;
#else
    path_prefix = "data/";
#endif
    printf("asset: path: %s\n", path_prefix);
#endif

    size_t needed = snprintf(NULL, 0, "%s%s", path_prefix, vpath) + 1;

    char* tmp = (char*)calloc(needed, 1);

    snprintf(tmp, needed, "%s%s", path_prefix, vpath);

    return tmp;
}


namespace spt
{

//	Packet create_player_packet(const Player& p)
//	{
//        Packet pa;
//        pa.type = PacketType::PLAYER_MOVE;
//
//        player_move_packet pmp = {
//                p.id,
//                (float )p.rect.x,
//                (float )p.rect.y
//		};
//
//        pa.data.resize(sizeof(player_move_packet));
//
//        auto wr = new WriteStream((uint8_t *)pa.data.data(), pa.data.size());
//
//        pmp.Serialize(wr);
//
//        return pa;
//	}
}

int main(int argc, char* argv[])
{
    std::string nickname;
    //std::cin >> nickname;


    //enet
    enet_initialize();

//	std::thread server_thread([]() {
//		spt::scope<Server> s = spt::create_scope<Server>();
//		s->Run();
//		});

    if(strcmp(argv[1], "server") == 0)
    {
        spt::scope<Server> s = spt::create_scope<Server>();
		s->Run();
    }


	EnetClient* c = new EnetClient();
	c->connect("127.0.0.1", "1234");
	new_player_packet pp{};
	Packet wp = WRAP_PACKET(PacketType::NEW_PLAYER, pp);
	//Packet wp = WRAP_PACKET(PacketType::NEW_PLAYER, pp);
	c->send(wp);

    //xddd
    std::vector<char> vc;
    Packet test ={};// WRAP_PACKET(2137, pp);
    test.type = 6;
    player_base_info_packet xd;
    xd.id = 0;

    player_base_info_packet xd2;
    //while (1)
    //{
    //    vc = spt::serialize(xd);
    //    xd2 = spt::deserialize<player_base_info_packet>(vc);
    //}

    spt::scope<Window> window;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* t1 = NULL;

    ASSERT_SDL(SDL_Init(SDL_INIT_EVERYTHING) >= 0)

    //Create window
    window = spt::create_scope<Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Splatter");

    //Create renderer
    renderer = SDL_CreateRenderer(window->get_ptr(), -1, SDL_RENDERER_ACCELERATED);
    ASSERT_SDL(renderer != NULL)

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    Texture* burgir = new Texture(get_real_path("burgir.png"), renderer);
    Texture* steak = new Texture(get_real_path("steak.png"), renderer);
    Texture* cookie = new Texture(get_real_path("cookie.png"), renderer);

    std::vector<Bullet*> bullets;

//    Player* gamer;
//    Player* other_gamer;
//    if(nickname.c_str()[0] == 'a')
//    {
//        gamer = new Player(burgir, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//        other_gamer = new Player(steak, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//    } else
//    {
//        other_gamer = new Player(burgir, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//        gamer = new Player(steak, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
//    }


    //here the player waits for the server to assign it unique id
    int t = PacketType::PACKET_EMPTY;
    while(t != PacketType::PLAYER_INFO)
    {
        wp = c->receive();
        t = wp.type;
    }
    int own_id = spt::deserialize<player_base_info_packet>(wp.data).id;

    printf("I have Id: %d\n", own_id);
   

    std::vector<Player*> players;

    while(t != PacketType::PLAYERS_POSITIONS)
    {
        wp = c->receive();
        t = wp.type;
    }

    auto players_positions_pckt =
        spt::deserialize<players_positions_packet>(wp.data);
        
    auto players_positions = players_positions_pckt.players;

    players.push_back(new Player(burgir, 0, 0, players_positions[0].id));

    players.push_back(new Player(steak, 0, 0, players_positions[1].id));
    
    bool quit = false;
    SDL_Event e;

    std::cout << "[CLIENT] Entering main loop!" << std::endl;
    while(!quit)
    {
        Uint64 start = SDL_GetPerformanceCounter();

        //Handle events
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x,&y);
                bullets.push_back(new Bullet(x, y, cookie, players[own_id]));
            }
        }

        auto packet = create_player_position_packet(*players[own_id]);
        Packet p = WRAP_PACKET(PacketType::PLAYER_POSITION, packet);

        c->send(p);

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        players[own_id]->Move(currentKeyStates);

        Packet rec;

        rec = c->receive();

        while (!rec.data.empty())
        {
            switch (rec.type) {
                case PacketType::PLAYER_POSITION:
                {
                    break;
                }
                case PacketType::PLAYERS_POSITIONS:
                {
                    auto temp =
                            spt::deserialize<players_positions_packet>(wp.data).players;

                    for(int i = 0; i < temp.size(); i++)
                    {
						if (temp[i].id != own_id)
                            handle_player_position_packet(temp[i], *players[i]);
                    }
                    break;
                }
            }
            rec = c->receive();
        }

        //Clear screen
        SDL_RenderClear(renderer);

        for (auto p_ : players)
        {
            p_->Render(renderer);
        }

        glm::vec2 b_pos;

        auto it = std::remove_if(bullets.begin(), bullets.end(), [](Bullet* b){
            glm::vec2 b_pos;
            b_pos = b->get_position();
            if(b_pos.x < 0 - b->GetWidth() || b_pos.x > SCREEN_WIDTH
               || b_pos.y < 0 - b->GetHeight() || b_pos.y > SCREEN_HEIGHT)
            {
                return true;
            }
            return false;
        });

        bullets.erase(it, bullets.end());

        for(auto b : bullets)
        {
            b->Move();
            b->Render(renderer);
        }

        //Update screen
        SDL_RenderPresent(renderer);

        Uint64 end = SDL_GetPerformanceCounter();

        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        auto delay = floor(16.666f - elapsedMS);

        SDL_Delay(delay < 16.666f && delay > 0 ? delay : 16.666f);
    }

#if WIN32
#include <Windows.h>
	TerminateThread(server_thread.native_handle(), 0);
	server_thread.detach();
#endif

	SDL_Quit();

    return EXIT_SUCCESS;
}

