#include <iostream>
#include <random>
#include <SDL.h>

#define CONST_FRAME_DELAY 6.944

#define RECT_MIN_WIDTH_HEIGHT 5
#define RECT_MAX_WIDTH_HEIGHT 50
#define RECT_MIN_X 0
#define RECT_MAX_X 640
#define RECT_Y -10

#define G 9.81
#define GRAVITY_SCALE 0.5

class Position
{
private:
	float x;
	float y;

public:
	Position(float xPos, float yPos)
	{
		x = xPos;
		y = yPos;
	}

	Position()
	{
		x = 0.f;
		y = 0.f;
	}

	float GetX() const { return x; }
	float GetY() const { return y; }

	void SetX(float newX)
	{
		x = newX;
	}
	void SetY(float newY)
	{
		y = newY;
	}
};

class Object
{
private:
	Position pos;
	SDL_Rect r;

	void SetPos(float x, float y)
	{
		// Update virtual coordinates
		pos.SetX(x);
		pos.SetY(y);

		// Update rect coordinates
		r.x = pos.GetX();
		r.y = pos.GetY();
	}
	void SetDimensions(float w, float h)
	{
		r.w = w;
		r.h = h;
	}

public:
	Object(SDL_Rect rect, Position p)
	{
		r = rect;
		pos = p;
		
		SetPos(p.GetX(), p.GetY());
	}

	Position GetPos() { return pos; }
	SDL_Rect GetRect() { return r; }

	void UpdatePos(float newX, float newY)
	{
		SetPos(newX, newY);
	}
};


void UpdateObjects(Object* object)
{
	object->UpdatePos(object->GetPos().GetX(), object->GetPos().GetY() + (G*GRAVITY_SCALE));
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
	std::uniform_int_distribution<int> size_dist(RECT_MIN_WIDTH_HEIGHT, RECT_MAX_WIDTH_HEIGHT);
	std::uniform_real_distribution<float> pos_dist(RECT_MIN_X, RECT_MAX_X);

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

				SDL_Rect r;

				// Set some defaults for rect, otherwise SDL cofetime for in go insane
				r.x = r.y = 0;

				r.w = r.h = size_rand;

				Position pos = Position(pos_rand, RECT_Y);
				Object* object = new Object(r, pos);
				objects.push_back(object);
			}

		}

		// Set background color 
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		// Set rect color here
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);

		// Update rectangles position

		for (auto& object : objects)
		{
			UpdateObjects(object);
			SDL_RenderFillRect(pRenderer, &object->GetRect());
			SDL_RenderPresent(pRenderer);
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