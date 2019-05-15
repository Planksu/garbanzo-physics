#include <iostream>
#include <random>
#include <SDL.h>
#include "Vector2.h"
#include "RGB.h"
#include "Object.h"

// 180 times per second = 5.555ms
#define CONST_PHYSICS_DELAY 8.33

// Window variables
#define WIDTH 1920
#define HEIGHT 1080

#define RECT_MIN_SIZE 75
#define RECT_MAX_SIZE 100
#define RECT_MIN_X 0
#define RECT_MAX_X WIDTH
#define RECT_Y -10
#define MASS_MIN 1
#define MASS_MAX 1

#define G 9.81
#define GRAVITY_SCALE 0.0005

#define DEBUG
//#define DEBUG_EXTENSIVE

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

float CrossProduct(Vector2 first, Vector2 second)
{
	return first.x * second.y - first.y * second.x;
}

Vector2 CrossProduct(Vector2 first, float s)
{
	return Vector2(s * first.y, -s * first.x);
}

Vector2 CrossProduct(float s, Vector2 first)
{
	return Vector2(-s * first.y, s * first.x);
}

#pragma endregion

#pragma region Collision methods
bool CheckAABBCollision(Object* first, Object* second)
{
#if defined DEBUG_EXTENSIVE
	Vector2 firstMax = first->GetBox().pos + first->GetBox().size;
	Vector2 secondMax = second->GetBox().pos + second->GetBox().size;
	Vector2 firstMin = first->GetBox().pos;
	Vector2 secondMin = second->GetBox().pos;
	std::cout << "First aabb pos: " << first->GetBox().pos.x << " " << first->GetBox().pos.y << std::endl;
	std::cout << "First aabb size: " << first->GetBox().size.x << " " << first->GetBox().size.y << std::endl;
	std::cout << "First max: " << firstMax.x << " " << firstMax.y << std::endl;
	std::cout << "First min: " << firstMin.x << " " << firstMin.y << std::endl;
	std::cout << "Second aabb pos: " << second->GetBox().pos.x << " " << second->GetBox().pos.y << std::endl;
	std::cout << "Second aabb size: " << second->GetBox().size.x << " " << second->GetBox().size.y << std::endl;
	std::cout << "Second max: " << secondMax.x << " " << secondMax.y << std::endl;
	std::cout << "Second min: " << secondMin.x << " " << secondMin.y << std::endl;
#endif

	if (first->GetBox().pos.x < second->GetBox().pos.x + second->GetBox().size.x &&
		first->GetBox().pos.x + first->GetBox().size.x > second->GetBox().pos.x &&
		first->GetBox().pos.y < second->GetBox().pos.y + second->GetBox().size.y &&
		first->GetBox().pos.y + first->GetBox().size.y > second->GetBox().pos.y)
	{
		return true;
	}

	return false;
}

void ResolveCollision(Object* a, Object* b)
{

	Vector2 ab = b->rb->velocity - a->rb->velocity;

	float ex = (a->GetBox().size.x / 2);
	float ey = (a->GetBox().size.y / 2);

	float dx = DotProduct(ab, Vector2(0, 1));

	if (dx > ex) dx = ex;
	if (dx < -ex) dx = -ex;

	float dy = DotProduct(ab, Vector2(1, 0));

	if (dy > ey) dy = ey;
	if (dy < -ey) dy = -ey;

	Vector2 r = Vector2(a->rb->position);
	Vector2 p = r + Vector2(0, 1) * dx + Vector2(1, 0)*dy;

	Vector2 collision_norm = Normalize(b->rb->position - p);

	float e = fmin(a->rb->restitution, b->rb->restitution);


#if defined DEBUG
	Vector2 startPoint = a->rb->position;
	Vector2 endPoint = startPoint + collision_norm * 500;
	SDL_RenderDrawLine(pRenderer, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
#endif

	a->rb->velocity = a->rb->velocity + (collision_norm * -1.f * e);
	b->rb->velocity = b->rb->velocity + (collision_norm * e);
}
#pragma endregion

void UpdateObjects(std::vector<Object*> &objects)
{
	for (auto& object : objects)
	{
		SDL_SetRenderDrawColor(pRenderer, object->GetColor().r, object->GetColor().g, object->GetColor().b, object->GetColor().a);
		
		// If the objects mass is set to 0, consider it as infinite mass and don't update
		if (object->rb->mass > 0.f)
		{
			// Add the gravity vector to force
			object->rb->force = object->rb->force + Vector2(0.f, G*GRAVITY_SCALE*object->rb->mass);
			object->rb->velocity = object->rb->velocity + object->rb->force * (1.0f / object->rb->mass + G);
			object->rb->position = object->rb->position + object->rb->velocity;

			object->UpdatePos();

#if defined DEBUG_EXTENSIVE
			std::cout << "Objects force: " << object->rb->force.x << ", " << object->rb->force.y << std::endl;
			std::cout << "Objects velocity: " << object->rb->velocity.x << ", " << object->rb->velocity.y << std::endl;
			std::cout << "Objects position: " << object->rb->position.x << ", " << object->rb->position.y << std::endl;
#endif

#if defined DEBUG
			// Top left to top right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y);
			// Top left to bottom left
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y, object->GetBox().pos.x, object->GetBox().pos.y + object->GetBox().size.y);
			// Bottom left to bottom right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y + object->GetBox().size.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y + object->GetBox().size.y);
			// Top right to bottom right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y + object->GetBox().size.y);
#else
			SDL_RenderFillRect(pRenderer, &object->GetRect());
#endif
			// REMEMBER TO SET FORCE TO ZERO AFTER APPLYING IT ON THIS UPDATE CYCLE
			object->rb->force = Vector2(0.f, 0.f);
		}
		else // Still draw objects that have infinite mass
		{
#if defined DEBUG
			// Top left to top right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y);
			// Top left to bottom left
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y, object->GetBox().pos.x, object->GetBox().pos.y + object->GetBox().size.y);
			// Bottom left to bottom right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x, object->GetBox().pos.y + object->GetBox().size.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y + object->GetBox().size.y);
			// Top right to bottom right
			SDL_RenderDrawLine(pRenderer, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y, object->GetBox().pos.x + object->GetBox().size.x, object->GetBox().pos.y + object->GetBox().size.y);
#else
			SDL_RenderFillRect(pRenderer, &object->GetRect());
#endif
		}
	}
}

void CreateObject(std::vector<Object*> &objects)
{
	// Setup vars for random c++11 style
	// Although techinally it is inefficient to create a new random engine every time we want a new object,
	// the performance hit is negligible. This is my preferred method as passing the random engines as method variables is impossible (?)
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> size_dist(RECT_MIN_SIZE, RECT_MAX_SIZE);
	std::uniform_real_distribution<float> pos_dist(RECT_MIN_X, RECT_MAX_X);
	std::uniform_real_distribution<float> mass_dist(MASS_MIN, MASS_MAX);
	std::uniform_int_distribution<int> color_1(0, 255);
	std::uniform_int_distribution<int> color_2(0, 255);
	std::uniform_int_distribution<int> color_3(0, 255);


	// Create object
	int size_rand = size_dist(generator);
	float pos_rand = pos_dist(generator);
	float mass_rand = mass_dist(generator);
	int color_1_rand = color_1(generator);
	int color_2_rand = color_2(generator);
	int color_3_rand = color_3(generator);

	RGB color = RGB(color_1_rand, color_2_rand, color_3_rand, 255);
	Vector2 pos = Vector2(pos_rand, RECT_Y);
	Rigidbody* rb = new Rigidbody(pos, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, mass_rand, 1.f, 0.f);
	Object* object = new Object(rb, color, Vector2(size_rand, size_rand));
	objects.push_back(object);
}

void CollisionCheck(std::vector<Object*> &objects)
{
	// Check for collisions
	for (int i = 0; i < objects.size(); i++)
	{
		// The j = i+1 prevents us from checking the same objects for collisions several times per frame
		// LINK: https://gamedev.stackexchange.com/a/24289 
		for (int j = i + 1; j < objects.size(); j++)
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
}

void RemoveUnseen(std::vector<Object*> &objects)
{
	// Remove objects that are not visible
	for (int i = 0; i < objects.size(); i++)
	{
		// If the objects y position is greater than the screen height, remove them as they're offscreen
		if (objects[i]->rb->position.y > HEIGHT)
		{
			// Object is out of screen, remove it
			std::vector<Object*>::iterator it = (objects.begin() + i);
			Object* objectToDelete = *(it);
			objects.erase(it);
			delete objectToDelete;
		}
	}
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
								WIDTH, HEIGHT,
								SDL_WINDOW_SHOWN);

	// Create renderer
	pRenderer = SDL_CreateRenderer(	pWindow,
									-1,	
									SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);



	std::vector<Object*> objects;

	// Set background color 
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(pRenderer);
	SDL_RenderPresent(pRenderer);

	SDL_Event e;
	bool quit = false;

	int counter = 0;

	for (size_t i = 1; i < 52; i++)
	{
		RGB color = RGB(255, 255, 255, 255);
		float widthChange = WIDTH / 50;
		Vector2 pos = Vector2(widthChange * i - widthChange/2, HEIGHT - 250 + 250 / 2);
		Rigidbody* rb = new Rigidbody(pos, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
		Object* object = new Object(rb, color, Vector2(widthChange, 250));
		objects.push_back(object);
	}
	

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
				CreateObject(objects);
			}
		}
		//if (counter % 25 == 0)
		//{
		//	CreateObject(objects);
		//}

		// Set background color 
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		UpdateObjects(objects);
		CollisionCheck(objects);
		RemoveUnseen(objects);

		SDL_RenderPresent(pRenderer);
		SDL_Delay(CONST_PHYSICS_DELAY);	
		counter++;
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