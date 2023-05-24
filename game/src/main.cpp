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
#include <algorithm>
#include <enet/enet.h>
#include <thread>

#define SCREEN_WIDTH 600//1280
#define SCREEN_HEIGHT 400//720

#define ASSERT_SDL(cond)    if(!(cond)){fprintf(stderr, "SDL broke: %s\n", SDL_GetError());return EXIT_FAILURE;}

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), exit(EXIT_FAILURE))

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


int main(int argc, char* argv[])
{




    //enet
    enet_initialize();

    if(strcmp(argv[1], "server") == 0)
    {
        EnetServer* s = new EnetServer();
        s->init("127.0.0.1", "1234", 2);
        char *data;
        int size;
        while (1)
        {
            data = s->receive(&size);
            if(data == NULL)
                continue;
            s->send_to_all(data, size);
        }
        return EXIT_SUCCESS;
    }

    EnetClient* c = new EnetClient();
    c->connect("127.0.0.1", "1234");

    char* my_data = (char*)calloc(1, sizeof(SDL_Rect) + sizeof(char));
    my_data[0] = argv[1][0];
    char* their_data;


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

    Texture* burgir = new Texture(get_real_path("burgir.png"), renderer);
    Texture* steak = new Texture(get_real_path("steak.png"), renderer);
    Texture* cookie = new Texture(get_real_path("cookie.png"), renderer);

    std::vector<Bullet*> bullets;

    Player* gamer;
    Player* other_gamer;
    if(argv[1][0] == 'a')
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

        //xd
        memcpy(my_data + 1, &gamer->rect, sizeof(SDL_Rect));
        c->send(my_data, sizeof(SDL_Rect) + sizeof(char));
        //xd

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        gamer->Move(currentKeyStates);


        while (1)
        {
            their_data = c->receive(NULL);
            if(their_data != NULL && their_data[0] != argv[1][0])
            {
                memcpy(&other_gamer->rect, their_data + 1, sizeof(SDL_Rect));
                break;
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

    return EXIT_SUCCESS;
}

