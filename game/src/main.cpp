#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION

#include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
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
#include "net/NetworkLayer.h"
#include <glm/glm.hpp>
#include <memory>
#include <algorithm>
#include <enet/enet.h>
#include <thread>
#include <iostream>
#include <server/Server.h>
#include <net/Packets.h>
#include <sstream>

#include <core/Window.h>


#include <lib/cereal/types/unordered_map.hpp>
#include <lib/cereal/types/memory.hpp>
#include <lib/cereal/archives/binary.hpp>
#include <lib/cereal/archives/portable_binary.hpp>
#include <core/Macros.h>
#include <core/Input.h>

SDL_Texture* loadTexture(std::string, SDL_Renderer*);

struct World {
    int local_bullet_idx = 0;
    std::vector<Bullet*> bullets;
	std::vector<Player*> players;
} world;

struct NetworkIdentity {
    int player_id;
    std::string nick;
    Player* owned_player;
} identity;

SDL_Renderer* renderer = NULL;



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

const char* get_real_path(std::string vpath)
{
    return get_real_path(vpath.c_str());
}

void net_update(int own_id, EnetClient* c)
{
    Packet rec;

    rec = c->receive();

    while (!rec.data.empty())
    {
        switch (rec.type) {
        case PacketType::PLAYER_POSITION:
        {
			auto temp =
				spt::deserialize<player_position_packet>(rec.data);


            // Received own position - discard TODO: CSP
            if (identity.owned_player->id == temp.id)
                break;

			for (int i = 0; i < world.players.size(); i++)
			{
				if (world.players[i]->id == temp.id)
					handle_player_position_packet(temp, *world.players[i]);
			}

            break;
        }
        case PacketType::PLAYERS_POSITIONS:
        {
            //auto temp =
            //    spt::deserialize<players_positions_packet>(rec.data).players;

            //for (int i = 0; i < temp.size(); i++)
            //{
            //    if (temp[i].id != own_id)
            //        handle_player_position_packet(temp[i], *players[i]);
            //}
            break;
        }
        case PacketType::PLAYER_INFO:
        {
		    int own_id = spt::deserialize<player_base_info_packet>(rec.data).id;
		    printf("I have Id: %d\n", own_id);
            break;
        }
        case PacketType::BULLET_POSITION_UPDATE:
        {

            break;
        }
        case PacketType::PLAYER_SPAWN:
        {
			auto np =
				spt::deserialize<new_player_packet>(rec.data);

            world.players.push_back(new Player(
                new Texture(get_real_path(std::string(np.avatar_texture_name.begin(), np.avatar_texture_name.end()) + ".png"),
                    renderer),
                15, 20, np.pid));
            break;
        }
        case PacketType::CLIENT_RECV_ID:
		{
            client_recv_id_packet pckt = spt::deserialize<client_recv_id_packet>(rec.data);
            identity.player_id = pckt.pid;
            identity.owned_player->id = pckt.pid;
        }

        }

        rec = c->receive();
    }

}

void net_connect(EnetClient* c, Texture* burgir, Texture* steak)
{
    if (!c->connect("127.0.0.1", "1234"))
        __debugbreak();

    Packet welcome_packet = WRAP_PACKET(PacketType::NEW_PLAYER, new_player_packet("burgir"));
    c->send(welcome_packet);

    identity.owned_player = new Player(new Texture(get_real_path(std::string("burgir") + ".png"), renderer), 15, 20, -1);
	world.players.push_back(identity.owned_player);
}

int main(int argc, char* argv[])
{
 //   SteamDatagramErrMsg msg;
	//if (!GameNetworkingSockets_Init(nullptr, msg))
	//	printf("GameNetworkingSockets_Init failed.  %s", msg);

    std::string nickname;
    //std::cin >> nickname;

    int own_id = -1;

    uint32_t tick;
    

    //enet
    enet_initialize();

#if WIN32

	std::thread server_thread([]() {


		spt::scope<Server> s = spt::create_scope<Server>();
		s->Run();
	});
#else
    if(strcmp(argv[1], "server") == 0)
    {
        spt::scope<Server> s = spt::create_scope<Server>();
		s->Run();
    }
#endif

    spt::scope<Input> input;
    spt::scope<Window> window;
    SDL_Texture* t1 = NULL;

    ASSERT_SDL(SDL_Init(SDL_INIT_EVERYTHING) >= 0)

    //Create window
    window = spt::create_scope<Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Splatter");
    input = spt::create_scope<Input>();

    //Create renderer
    renderer = SDL_CreateRenderer(window->get_ptr(), -1, SDL_RENDERER_ACCELERATED);
    ASSERT_SDL(renderer != NULL)

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    Texture* burgir = new Texture(get_real_path("burgir.png"), renderer);
    Texture* steak = new Texture(get_real_path("steak.png"), renderer);
    Texture* cookie = new Texture(get_real_path("cookie.png"), renderer);

	EnetClient* c = new EnetClient();


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
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
                input->update_mouse_wheel(e.wheel);
            }
        }

        input->update(e);

        if (c->is_connected)
        {
            net_update(own_id, c);

			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

			if (identity.owned_player->id != -1)
			{

				identity.owned_player->Move(currentKeyStates);

				auto packet = create_player_position_packet(*identity.owned_player);
				Packet p = WRAP_PACKET(PacketType::PLAYER_POSITION, packet);
				c->send(p);
			}

            if (Input::mouse_down(0))
            {
                world.bullets.push_back(new Bullet(Input::get_mouse_pos().x, Input::get_mouse_pos().y, cookie, identity.owned_player,
                    identity.player_id, world.local_bullet_idx++));
                
                // notify other players about new bullet spawn



            }
        }
        else if (Input::key_down(SDL_SCANCODE_U))
			net_connect(c, burgir, steak);

        //Clear screen
        SDL_RenderClear(renderer);

        for (auto p_ : world.players)
        {
            p_->Render(renderer);
        }

        glm::vec2 b_pos;

        // remove bullets outside of screen area
        auto it = std::remove_if(world.bullets.begin(), world.bullets.end(), [](Bullet* b){
            glm::vec2 b_pos;
            b_pos = b->get_position();
            if(b_pos.x < 0 - b->GetWidth() || b_pos.x > SCREEN_WIDTH
               || b_pos.y < 0 - b->GetHeight() || b_pos.y > SCREEN_HEIGHT)
            {
                return true;
            }
            return false;
        });

        world.bullets.erase(it, world.bullets.end());

        for(auto b : world.bullets)
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

