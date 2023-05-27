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
#include <PacketTypes.h>
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



struct player_move_packet {
	int x;
	int y;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(x, y);
	}
};


namespace spt
{

	Packet create_player_packet(const Player& p)
	{
        Packet pa;
        pa.type = PacketType::PLAYER_MOVE;

        player_move_packet pmp = {
			p.rect.x, p.rect.y
		};

        pa.data = serialize<player_move_packet>(pmp);
        pa.size = pa.data.size();

        return pa;
	}
}


void handle_player_move_packet(Packet& pa, Player* player)
{
	player_move_packet p = spt::deserialize<player_move_packet>(pa.data);

    player->rect.x = p.x;
    player->rect.y = p.y;
}

int main(int argc, char* argv[])
{
    std::string nickname;
    std::cin >> nickname;


    //enet
    enet_initialize();

    std::thread server_thread([](){
		spt::scope<Server> s = spt::create_scope<Server>();
		s->Run();
    });


    EnetClient* c = new EnetClient();
    c->connect("127.0.0.1", "1234");

    char* my_data = (char*)calloc(1, sizeof(SDL_Rect) + sizeof(char));
    my_data[0] = nickname.c_str()[0];
    
    Packet their_data;

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

    Player* gamer;
    Player* other_gamer;
    if(nickname.c_str()[0] == 'a')
    {
        gamer = new Player(burgir, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        other_gamer = new Player(steak, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    } else
    {
        other_gamer = new Player(burgir, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        gamer = new Player(steak, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    }

    bool quit = false;
    SDL_Event e;

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
                bullets.push_back(new Bullet(x, y, cookie, gamer));
            }
        }

        auto packet = spt::create_player_packet(*gamer);



        c->send(packet);

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        gamer->Move(currentKeyStates);

        Packet rec;

        rec = c->receive();

        if (rec.data.size() > 0)
        {

            switch (rec.type) {
            case PacketType::PLAYER_MOVE:
                handle_player_move_packet(rec, other_gamer);
            }
        }

        //Clear screen
        SDL_RenderClear(renderer);

        gamer->Render(renderer);
        other_gamer->Render(renderer);

        glm::vec2 b_pos;

        auto it = std::remove_if(bullets.begin(), bullets.end(), [](Bullet* b){
            glm::vec2 b_pos;
            b_pos = b->get_position();
            if(b_pos.x < 0 - b->rect.w || b_pos.x > SCREEN_WIDTH
               || b_pos.y < 0 - b->rect.h || b_pos.y > SCREEN_HEIGHT)
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

