#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#include <gfx/Texture.h>
#include <SDL3/SDL.h>
#include <enet/enet.h>
#include <glm/glm.hpp>

#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <sstream>

#include <core/Window.h>
#include <core/Macros.h>
#include <core/Input.h>
#include <core/Asset.h>


int main(int argc, char* argv[])
{
    SDL_Renderer* renderer = NULL;
    
    //enet
    enet_initialize();

    spt::scope<Input> input;
    spt::scope<Window> window;


    ASSERT_SDL(SDL_Init(0) >= 0)

    //Create window
    window = spt::create_scope<Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Splatter");
    input = spt::create_scope<Input>();

    //Create renderer
    renderer = SDL_CreateRenderer(window->get_ptr(), NULL);
    
    ASSERT_SDL(renderer != NULL)
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    bool quit = false;
    SDL_Event e;

    std::cout << "[CLIENT] Entering main loop!" << std::endl;
    while(!quit)
    {
        Uint64 start = SDL_GetPerformanceCounter();

        //Handle events
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
            else if(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                float x, y;
                SDL_GetMouseState(&x,&y);
            }
            else if (e.type == SDL_EVENT_MOUSE_WHEEL)
            {
                input->update_mouse_wheel(e.wheel);
            }
        }

        input->update(e);

        //Clear screen
        SDL_RenderClear(renderer);

 
 
        //Update screen
        SDL_RenderPresent(renderer);

        Uint64 end = SDL_GetPerformanceCounter();

        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        auto delay = floor(16.666f - elapsedMS);

        SDL_Delay(delay < 16.666f && delay > 0 ? delay : 16.666f);
    }

	SDL_Quit();
    return EXIT_SUCCESS;
}

