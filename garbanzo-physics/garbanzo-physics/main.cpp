#include <iostream>
#include <SDL.h>

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

	// Draw rect
	SDL_Rect r;
	r.x = r.y = r.w = r.h = 50;

	// Set rect color here
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);

	SDL_RenderFillRect(pRenderer, &r);

	SDL_RenderPresent(pRenderer);

	SDL_Delay(5000);

	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}