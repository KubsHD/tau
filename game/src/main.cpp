#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION

#define GLM_FORCE_XYZW_ONLY

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
#include <game/Player.h>
#include <game/Bullet.h>
#include "net/NetworkLayer.h"
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <memory>
#include <algorithm>
#include <enet/enet.h>
#include <thread>
#include <iostream>
#include <server/Server.h>
#include <net/Packets.h>
#include <sstream>


#include <core/Window.h>

#include <gfx/Renderer.h>

#include <core/Macros.h>
#include <core/Input.h>
#include <core/Asset.h>
#include <gfx/api/Device.h>

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

spt::scope<Device> ren;

static float vertices[] = {
	// pos      // tex
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

spt::ref<Texture> load_and_create_texture(const spt::scope<Device>& dev, std::string path)
{
	int channels, width, height;

	stbi_uc* data = stbi_load(Asset::get_real_path(path), &width, &height, &channels, 0);

	return dev->create_texture({
		.name = path,
		.size = {width, height},
		.format = ColorFormat::RGBA8_SRGB,
		.data = std::vector<char>(data, data + (width * height * channels))
		});
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
                load_and_create_texture(ren, std::string(np.avatar_texture_name.begin(), np.avatar_texture_name.end()) + ".png"),
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

void net_connect(EnetClient* c)
{
    if (!c->connect("127.0.0.1", "1234"))
        {
        //__debugbreak();
        }
    Packet welcome_packet = WRAP_PACKET(PacketType::NEW_PLAYER, new_player_packet("burgir"));
    c->send(welcome_packet);

    identity.owned_player = new Player(load_and_create_texture(ren, (std::string("burgir") + ".png")), 15, 20, -1);
	world.players.push_back(identity.owned_player);
}

struct MVP {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::mat4 mvp;

    inline void calc()
    {
        mvp = glm::transpose(projection * view * model);
    }
};


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

#if WIN32 || 1

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
    ren = spt::create_scope<Device>(window->get_ptr());

    //Create renderer
    //renderer = SDL_CreateRenderer(window->get_ptr(), -1, SDL_RENDERER_ACCELERATED);
    //ASSERT_SDL(renderer != NULL)

    //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    //STexture* cookie = new STexture(Asset::get_real_path("cookie.png"), renderer);

	EnetClient* c = new EnetClient();

	static float vertices2[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
    
    auto cookie2 = load_and_create_texture(ren, "cookie.png");


    auto default_pipeline = ren->create_pipeline({
        .vertexShader = "shaders/sprite.hlsl",
        .pixelShader = "shaders/sprite.hlsl",
    });

    auto quadVertexBuffer = ren->create_buffer({
        .bindFlags = BindFlags::BIND_VERTEX_BUFFER,
        .byteWidth = sizeof(vertices2),
        .data = vertices2
    });


    MVP p = {
        .model = glm::scale(glm::mat4(1.0f), glm::vec3(32, 32, 1)),
        .view = glm::mat4(1.0f),
        .projection = glm::ortho(0.0f, 600.0f, 400.0f, 0.0f, -1.0f, 1.0f),
        .mvp = glm::transpose(p.projection * p.model * p.view)
    };

	auto mvpBuffer = ren->create_buffer({
		.bindFlags = BindFlags::BIND_CONSTANT_BUFFER,
		.byteWidth = sizeof(MVP),
		.data = &p
	});


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
                world.bullets.push_back(new Bullet(Input::get_mouse_pos().x, Input::get_mouse_pos().y, cookie2, identity.owned_player,
                    identity.player_id, world.local_bullet_idx++));
                
                // notify other players about new bullet spawn



            }
        }
        else if (Input::key_down(SDL_SCANCODE_U))
			net_connect(c);

        ren->clear();


        // texture drawing
        //ren->draw_texture(default_pipeline, quadVertexBuffer, cookie2, { 0, 0}, { 0, 0 });

        DrawData dat;
        dat.pipeline = default_pipeline;
		dat.texture = cookie2;
		dat.uniformBuffer = mvpBuffer;
		dat.vertexBuffer = quadVertexBuffer;
        dat.vertexCount = 6;
		dat.vertexStride = sizeof(float) * 4;
        dat.vertexOffset = 0;

        p.model = glm::translate(p.model, glm::vec3(0.1f,0.1f,0));
        p.calc();

        ren->update_buffer(mvpBuffer, &p, sizeof(p));
        ren->submit_draw(dat);

        ////Clear screen
        //SDL_RenderClear(renderer);

		for (const auto& p_ : world.players)
		{
			DrawData dat;
			dat.pipeline = default_pipeline;
			dat.texture = cookie2;
			dat.uniformBuffer = mvpBuffer;
			dat.vertexBuffer = quadVertexBuffer;
			dat.vertexCount = 6;
			dat.vertexStride = sizeof(float) * 4;
			dat.vertexOffset = 0;

			p.model = glm::translate(p.model, glm::vec3(p_->GetX(), p_->GetY(), 0));
			p.calc();

			ren->update_buffer(mvpBuffer, &p, sizeof(p));
			ren->submit_draw(dat);
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

        //for(auto b : world.bullets)
        //{
        //    b->Move();
        //    b->Render(renderer);
        //}

        ////Update screen
        //SDL_RenderPresent(renderer);

        ren->commit();
        ren->swap();

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

