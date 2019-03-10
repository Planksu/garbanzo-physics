#include <iostream>
#include <random>
#include <SDL.h>

#define CONST_FRAME_DELAY 16.67
#define RECT_MIN_WIDTH_HEIGHT 5
#define RECT_MAX_WIDTH_HEIGHT 250

int Clamp(int value, int low, int high)
{
	return std::max(low, std::min(value, high));
}

int main(int argc, char * argv[])
{
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		std::cout << "SDL initialization succeeded!";
	}

	// Create window
	SDL_Window* pWindow = NULL;
	pWindow = SDL_CreateWindow("Garbanzo-physics demo",
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								800, 600,
								SDL_WINDOW_SHOWN);

	// Create renderer
	SDL_Renderer* pRenderer = NULL; 
	pRenderer = SDL_CreateRenderer(	pWindow,
									-1,	
									SDL_RENDERER_ACCELERATED);

	// Set background color 
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(pRenderer);

	// Setup vars for random c++11 style
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(RECT_MIN_WIDTH_HEIGHT, RECT_MAX_WIDTH_HEIGHT);
	int rand = distribution(generator);

	SDL_Event e;
	bool quit = false;
	
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
				break;
			}

			// Draw rect
			SDL_Rect r;
			r.x = 400;
			r.y = 300;

			r.w = rand;
			r.h = rand;

			// Set rect color here
			SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
			SDL_RenderFillRect(pRenderer, &r);
			SDL_RenderPresent(pRenderer);


			SDL_Delay(CONST_FRAME_DELAY);

			std::cout << "Loop" << std::endl;
		}
	}
	

	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}