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
#include <glm/glm.hpp>
#include <algorithm>
#include <enet/enet.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define ASSERT_SDL(cond)    if(!(cond)){fprintf(stderr, "SDL broke: %s\n", SDL_GetError());return EXIT_FAILURE;}

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), exit(EXIT_FAILURE))

SDL_Texture* loadTexture(std::string, SDL_Renderer*);

//static std::vector<ENetPeer*> g_remote_peers;

int main(int argc, char* argv[])
{

    //enet
    enet_initialize();

    ENetHost* client;

    client = enet_host_create(NULL, 1, 2, 0, 0);

    if(argv[1] == "server")
    {
        ENetAddress address;
        ENetHost* server;

        address.host = ENET_HOST_ANY;
        address.port = 1234;

        server = enet_host_create(&address, 2, 2, 0, 0);
    }


    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* t1 = NULL;

    ASSERT_SDL(SDL_Init(SDL_INIT_EVERYTHING) >= 0)

    //Create window
    window = SDL_CreateWindow("Splatter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    ASSERT_SDL(window != NULL)

    //Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ASSERT_SDL(renderer != NULL)

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    Texture* burgir = new Texture("./../../game/data/burgir.png", renderer);
    Texture* cookie = new Texture("./../../game/data/cookie.png", renderer);

    std::vector<Bullet*> bullets;

    Player* gamer = new Player(burgir, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

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

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        gamer->Move(currentKeyStates);

        //Clear screen
        SDL_RenderClear(renderer);

        gamer->Render(renderer);

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

        SDL_Delay(floor(16.666f - elapsedMS));
    }

    return EXIT_SUCCESS;
}

