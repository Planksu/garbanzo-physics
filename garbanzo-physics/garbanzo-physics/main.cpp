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

SDL_Renderer* pRenderer = NULL;

#pragma region Math functions
float DotProduct(Vector2 first, Vector2 second)
{
	return (first.x * second.x) + (first.y * second.y);
}

Vector2 Normalize(Vector2 input)
{
	float length = sqrt(input.x * input.x + input.y * input.y);

	return Vector2(input.x / length, input.y / length);
}

bool CheckAABBCollision(Object* first, Object* second)
{
	/*std::cout << "First aabb pos: " << first->GetBox().pos.x << " " << first->GetBox().pos.y << std::endl;
	std::cout << "First aabb size: " << first->GetBox().size.x << " " << first->GetBox().size.y << std::endl;*/

	Vector2 firstMax = first->GetBox().pos + first->GetBox().size;

	/*std::cout << "First max: " << firstMax.x << " " << firstMax.y << std::endl;*/

	Vector2 secondMax = second->GetBox().pos + second->GetBox().size;
	Vector2 firstMin = first->GetBox().pos;

	/*std::cout << "First min: " << firstMin.x << " " << firstMin.y << std::endl;*/

	Vector2 secondMin = second->GetBox().pos;


	//if (firstMax.x > secondMin.x && firstMax.y > secondMin.y)
	//{
	//	std::cout << "FirstMax.x: " << firstMax.x << ", " << "SecondMin.x: " << secondMin.x << ", " << "FirstMax.y: " << firstMax.y << ", " << "SecondMin.y:" << secondMin.y << std::endl;

	//	std::cout << "First type of collision happened!" << std::endl;
	//}

	//if (firstMin.x < secondMax.x && firstMin.y < secondMax.y)
	//{
	//	std::cout << "Second type of collision happened!" << std::endl;
	//}

	if (first->GetBox().pos.x < second->GetBox().pos.x + second->GetBox().size.x &&
		first->GetBox().pos.x + first->GetBox().size.x > second->GetBox().pos.x &&
		first->GetBox().pos.y < second->GetBox().pos.y + second->GetBox().size.y &&
		first->GetBox().pos.y + first->GetBox().size.y > second->GetBox().pos.y)
	{
		//std::cout << "END OF METHOD\n\n" << std::endl;
		return true;
	}

	//if (/*(firstMax.x > secondMin.x && firstMax.y > secondMin.y) || */(firstMin.x < secondMax.x && firstMin.y < secondMax.y))
	//{

	//}

	return false;
}
#pragma endregion

void UpdateObjects(Object* object)
{
	object->UpdatePos(object->GetPos().x + object->GetVelocity().x, object->GetPos().y + (G*GRAVITY_SCALE*object->GetMass()) + object->GetVelocity().y);
}

void ResolveCollision(Object* a, Object* b)
{
	Vector2 ab = b->GetVelocity() - a->GetVelocity();
	
	float ex = (a->GetRect().w/2);
	float ey = (a->GetRect().h/2);

	float dx = DotProduct(ab, Vector2(0, 1));

	if (dx > ex) dx = ex;
	if (dx < -ex) dx = -ex;

	float dy = DotProduct(ab, Vector2(1, 0));

	if (dy > ey) dy = ey;
	if (dy < -ey) dy = -ey;

	Vector2 r = Vector2(a->GetPos());
	Vector2 p = r + Vector2(0,1) * dx + Vector2(1, 0)*dy;

	Vector2 collision_norm = Normalize(b->GetPos() - p);

	float e = fmin(a->GetRestitution(), b->GetRestitution());

	//std::cout << collision_norm.x << " " << collision_norm.y << std::endl;

	Vector2 startPoint = a->GetPos();
	Vector2 endPoint = startPoint + collision_norm * 500;
	SDL_RenderDrawLine(pRenderer, startPoint.x, startPoint.y, endPoint.x, endPoint.y);

	a->SetVelocity(collision_norm * -1.f * e);
	b->SetVelocity(collision_norm * e);
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
				Object* object = new Object(r, pos, color, mass_rand, 1.f);
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
			
		}


		// Check for collisions
		for (int i = 0; i < objects.size(); i++)
		{
			for (int j = i+1; j < objects.size(); j++)
			{
				// If index is same, don't check for collisions with self
				if (i != j)
				{
					if (CheckAABBCollision(objects[i], objects[j]))
					{
						ResolveCollision(objects[i], objects[j]);
					}
				}
			}
		}

		// Remove objects that are not visible
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i]->GetPos().y > 600)
			{
				// Object is out of screen, remove it
				std::vector<Object*>::iterator it = (objects.begin() + i);
				Object* objectToDelete = *(it);
				objects.erase(it);
				delete objectToDelete;
			}
		}

		SDL_RenderPresent(pRenderer);

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