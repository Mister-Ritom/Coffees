#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <random>

bool checkCollision(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    // Check if the rectangles overlap
    return (rect1.x < rect2.x + rect2.w &&
        rect1.x + rect1.w > rect2.x &&
        rect1.y < rect2.y + rect2.h &&
        rect1.y + rect1.h > rect2.y);
}


int SDL_main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL Image
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "Failed to initialize SDL Image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("Coffees", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create an SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the PNG image
    SDL_Surface* backgroundSurface = IMG_Load("textures/background.png");
    SDL_Surface* playerSurface = IMG_Load("textures/player.png");
    SDL_Surface* beanSurface = IMG_Load("textures/bean.png");
    if (backgroundSurface == nullptr)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create an SDL texture from the image surface
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_Texture* beanTexture = SDL_CreateTextureFromSurface(renderer, beanSurface);
    if (backgroundTexture == nullptr)
    {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(backgroundSurface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Rect player = { 350,250,50,50 };
    std::vector<SDL_Rect> beans;

    // Wait for the user to close the window
    bool quit = false;
    int lastTick = 0;
    bool mouseClicked = false;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distY(50, 550);
    std::uniform_int_distribution<int> distX(400, 750);
    int x = 0;
    while (!quit)
    {
        mouseClicked = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = true;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (player.y>10)
                        player.y -= 50;
                    mouseClicked = true;
                }
            }
        }

        if (beans.empty()) {
            x = 0;
            SDL_Rect bean1 = { distX(gen),distY(gen) ,25,25 };
            SDL_Rect bean2 = { distX(gen),distY(gen) ,25,25 };

            beans.push_back(bean1);
            beans.push_back(bean2);

        }

        if (lastTick == 30) {

            if (player.y < 550 && !mouseClicked)
                player.y += 5;
            x++;
            lastTick = 0;
        }
        else {
            lastTick++;
        }

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Render the texture to the screen
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        //Render player
        SDL_RenderCopy(renderer, playerTexture, nullptr, &player);

        //Render beans
        for (int i = 0; i < beans.size(); i++) {
            SDL_Rect bean = beans[i];
            bean.x -= x;
            SDL_RenderCopy(renderer, beanTexture, nullptr, &bean);
            if (checkCollision(player, bean)) {
                beans.erase(beans.begin() + i);
                i--;
            }
            if (bean.x < 0) {
                beans.erase(beans.begin() + i);
                i--;
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);

    }

    // Clean up and quit SDL
    SDL_DestroyTexture(backgroundTexture);
    SDL_FreeSurface(backgroundSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
