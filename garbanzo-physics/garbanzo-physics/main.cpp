#include <iostream>
#include <random>
#include <SDL.h>
#include "Vector2.h"
#include "RGB.h"
#include "Object.h"

// 6.944 = 144fps, 16.67 = 60fps
#define CONST_FRAME_DELAY 16.67

#define RECT_MIN_SIZE 25
#define RECT_MAX_SIZE 150
#define RECT_MIN_X 0
#define RECT_MAX_X 640
#define RECT_Y -10
#define MASS_MIN 0.5
#define MASS_MAX 5

#define G 9.81
#define GRAVITY_SCALE 0.1

#pragma region Math functions
float DotProduct(Vector2 first, Vector2 second)
{
	return (first.GetX() * second.GetX()) + (first.GetY() * second.GetY());
}

bool CheckAABBCollision(AABB first, AABB second)
{
	if (first.max.GetX() < second.min.GetX() || first.min.GetX() > second.max.GetX()) return false;
	if (first.max.GetY() < second.min.GetY() || first.min.GetY() > second.max.GetY()) return false;

	return true;
}
#pragma endregion

void UpdateObjects(Object* object)
{
	object->UpdatePos(object->GetPos().GetX(), object->GetPos().GetY() + (G*GRAVITY_SCALE*object->GetMass()));
}

void ResolveCollision(Object* a, Object* b)
{
	Vector2 ab = b->GetVelocity() - a->GetVelocity();
	float velocityAlongNormal = DotProduct(ab, )
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
	pWindow = SDL_CreateWindow("Garbanzo physics demo",
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								800, 600,
								SDL_WINDOW_SHOWN);

	// Create renderer
	SDL_Renderer* pRenderer = NULL; 
	pRenderer = SDL_CreateRenderer(	pWindow,
									-1,	
									SDL_RENDERER_ACCELERATED);


	// Setup vars for random c++11 style
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> size_dist(RECT_MIN_SIZE, RECT_MAX_SIZE);
	std::uniform_real_distribution<float> pos_dist(RECT_MIN_X, RECT_MAX_X);
	std::uniform_real_distribution<float> mass_dist(MASS_MIN, MASS_MAX);

	std::vector<Object*> objects;

	// Set background color 
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(pRenderer);
	SDL_RenderPresent(pRenderer);

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
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
			{
				// Create object

				int size_rand = size_dist(generator);
				float pos_rand = pos_dist(generator);
				float mass_rand = mass_dist(generator);

				SDL_Rect r;

				// Set some defaults for rect, otherwise SDL cofetime for in go insane
				r.x = r.y = 0;

				r.w = r.h = size_rand;

				RGB color = RGB(0, 255, 255, 255);
				Vector2 pos = Vector2(pos_rand, RECT_Y);
				Object* object = new Object(r, pos, color, mass_rand);
				objects.push_back(object);
			}

		}

		// Set background color 
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		// Update rectangles position
		for (auto& object : objects)
		{
			SDL_SetRenderDrawColor(pRenderer, object->GetColor().r, object->GetColor().g, object->GetColor().b, object->GetColor().a);
			UpdateObjects(object);
			SDL_RenderFillRect(pRenderer, &object->GetRect());
			SDL_RenderPresent(pRenderer);
		}

		// Check for collisions
		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = 0; j < objects.size(); j++)
			{
				// If index is same, don't check for collisions with self
				if (i != j)
				{
					if (CheckAABBCollision(objects[i]->GetBox(), objects[j]->GetBox()))
					{
						ResolveCollision(objects[i], objects[j]);
					}
				}
			}
		}

		// Remove objects that are not visible
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i]->GetPos().GetY() > 600)
			{
				// Object is out of screen, remove it
				std::vector<Object*>::iterator it = (objects.begin() + i);
				Object* objectToDelete = *(it);
				objects.erase(it);
				delete objectToDelete;
			}
		}

		SDL_Delay(CONST_FRAME_DELAY);	
	}
	
	// Clear allocated memory
	for (auto& object : objects)
	{
		delete object;
	}

	// Clean up
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	

	return 0;
}