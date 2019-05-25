#include <iostream>
#include <random>
#include <SDL.h>
#include "Vector2.h"
#include "RGB.h"
#include "Object.h"

// 180 times per second = 5.555ms
#define CONST_PHYSICS_DELAY 5.55

// Window variables
#define WIDTH 1920
#define HEIGHT 1080

#define RECT_MIN_SIZE 75
#define RECT_MAX_SIZE 100
#define RECT_MIN_X 0
#define RECT_MAX_X WIDTH
#define RECT_MIN_Y 10
#define RECT_MAX_Y 150
#define MASS_MIN 10
#define MASS_MAX 30

#define G 9.81
#define GRAVITY_SCALE 1


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

// A primitive AABB collision check, not used anymore after the angular velocities came in and ruined everything
// The SAT collision check right below this one is the one that is used right now
bool CheckAABBCollision(Object* first, Object* second)
{
	if (first->GetBox().topLeft.x < second->GetBox().topLeft.x + second->GetBox().size.x &&
		first->GetBox().topLeft.x + first->GetBox().size.x > second->GetBox().topLeft.x &&
		first->GetBox().topLeft.y < second->GetBox().topLeft.y + second->GetBox().size.y &&
		first->GetBox().topLeft.y + first->GetBox().size.y > second->GetBox().topLeft.y)
	{
		return true;
	}

	return false;
}

bool CheckSatCollision(Object* first, Object* second)
{
	// Honestly, I wrote these this way because I was debugging the whole SAT thing for like 15 hours and
	// I can't be bothered to write something else for it
	std::vector<Vector2> firstCorners;	
	firstCorners.push_back(first->GetBox().topLeft);
	firstCorners.push_back(first->GetBox().topRight);
	firstCorners.push_back(first->GetBox().bottomRight);
	firstCorners.push_back(first->GetBox().bottomLeft);

	std::vector<Vector2> secondCorners;
	secondCorners.push_back(second->GetBox().topLeft);
	secondCorners.push_back(second->GetBox().topRight);
	secondCorners.push_back(second->GetBox().bottomRight);
	secondCorners.push_back(second->GetBox().bottomLeft);

	// In a SAT collision check, we need the four unique axis involved in a collision
	// The four here are upwards and left of both colliding objects
	// In addition, the normals will point inwards but as long as all of the normals point inwards it doesn't matter
	std::vector<Vector2> axis;
	Vector2 firstAxis = first->rb->position - first->GetBox().topNormal;
	Vector2 secondAxis = first->rb->position - first->GetBox().leftNormal;
	Vector2 thirdAxis = second->rb->position - second->GetBox().topNormal;
	Vector2 fourthAxis = second->rb->position - second->GetBox().leftNormal;

	// By creating the "normals" like this, they both point inward into the object instead of outwards like normally
	// This doesn't matter as long as both of the axis' are the same way
	axis.push_back(Normalize(firstAxis));
	axis.push_back(Normalize(secondAxis));
	axis.push_back(Normalize(thirdAxis));
	axis.push_back(Normalize(fourthAxis));

	float minOverlap = std::numeric_limits<float>::infinity();

	// Project every corner onto every axis
	// If all projections overlap, a collision is happening
	for (size_t i = 0; i < axis.size(); i++)
	{
		float overlap;

		float aMin = std::numeric_limits<float>::infinity();
		float aMax = -std::numeric_limits<float>::infinity();
		float bMin = std::numeric_limits<float>::infinity();
		float bMax = -std::numeric_limits<float>::infinity();

		for (size_t j = 0; j < firstCorners.size(); j++)
		{
			float result = DotProduct(axis[i], firstCorners[j]);
			if (result > aMax)
				aMax = result;
			if (result < aMin)
				aMin = result;
		}
		for (size_t j = 0; j < secondCorners.size(); j++)
		{
			float result = DotProduct(axis[i], secondCorners[j]);
			if (result > bMax)
				bMax = result;
			if (result < bMin)
				bMin = result;
		}

		// If a separating axis is found, a collision can't be possible so return false straight away
		if (bMin > aMax || bMax < aMin)
		{
			return false;
		}
		else // If this axis is not separating, take note of the overlap it has
		{
			overlap = std::min(aMax, bMax) - std::max(aMin, bMin);
		}

		if (overlap != 0.f && overlap < minOverlap)
		{
			minOverlap = overlap;

			// Ideally this would be done only once
			// But it's not a really expensive calculation
			first->mtv = axis[i] * minOverlap;

			// If center offset and overlap aren't pointing in the same direction, reverse mtv
			// Without this check the objects would be sucked into the middle instead of colliding away
			if (DotProduct((first->rb->position - second->rb->position), first->mtv) > 0)
			{
				first->mtv = first->mtv * -1.f;
			}
		}
	}
	// If we've reached this point, no separating axis is found so a collision is happening
	return true;
}

void ResolveCollision(Object* a, Object* b)
{
	// Linear part

	float aMass;
	float bMass;
	// Check if the mass is 0, if it is the mass is to be considered infinite
	// If for some reason the mass is negative it will be considered infinite too
	if (a->rb->mass > 0)
		aMass = a->rb->mass;
	else
		aMass = 100000;

	if (b->rb->mass > 0)
		bMass = b->rb->mass;
	else
		bMass = 100000;

	// Get vector from one object to another
	Vector2 ab = b->rb->velocity - a->rb->velocity;
	// Collision normal is obtained from SAT collision check and the value is stored in the A-objects mtv-variable
	Vector2 normal = Normalize(a->mtv);

	// Check if velocity will separate the objects next frame, no collision required then
	float contactVel = DotProduct(ab, normal);
	if (contactVel > 0)
		return;

#if defined DEBUG_EXTENSIVE
	SDL_SetRenderDrawColor(pRenderer, 0, 255, 255, 255);
	Vector2 startPoint = a->rb->position;
	Vector2 endPoint = startPoint + normal * 500;
	SDL_RenderDrawLine(pRenderer, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
#endif

	// Calculate impulse and velocity
	float e = fmin(a->rb->restitution, b->rb->restitution);
	float j = -(1.0f + e) * contactVel;
	j /= 1 / aMass + 1 / bMass;

	Vector2 impulse = normal * j;
	a->rb->velocity = a->rb->velocity - impulse * (1 / aMass);
	b->rb->velocity = b->rb->velocity + impulse * (1 / bMass);
	
	// Angular part
	// Rough estimation of the "arm" vectors by getting vector from one object to another and dividing it by two
	Vector2 bNormal = Normalize(b->rb->position - a->rb->position);
	Vector2 bArmVector = bNormal * sqrt(b->GetBox().size.x * b->GetBox().size.x + b->GetBox().size.y * b->GetBox().size.y);
	bArmVector = b->rb->position - bArmVector;
	Vector2 aNormal = Normalize(a->rb->position - b->rb->position);
	Vector2 aArmVector = aNormal * sqrt(a->GetBox().size.x * a->GetBox().size.x + a->GetBox().size.y * a->GetBox().size.y);
	aArmVector = a->rb->position - aArmVector;

	// Cross product to calculate the torque
	a->rb->torque = aArmVector.x * a->rb->force.y - aArmVector.y * a->rb->force.x;
	b->rb->torque = bArmVector.x * b->rb->force.y - bArmVector.y * b->rb->force.x;

	// Here we check if the torque applied should be positive or negative
	// In this simulation, positive torque applies rotation clockwise, and negative counter-clockwise
	if (aNormal.x < 0)
		a->rb->torque = -a->rb->torque;
	if (bNormal.x < 0)
		b->rb->torque = -b->rb->torque;
}
#pragma endregion

// This method just draws lines between the four corners of the object
void DrawObject(Object* o)
{
	SDL_RenderDrawLine(pRenderer, o->GetBox().topLeft.x, o->GetBox().topLeft.y, o->GetBox().topRight.x, o->GetBox().topRight.y);
	SDL_RenderDrawLine(pRenderer, o->GetBox().topLeft.x, o->GetBox().topLeft.y, o->GetBox().bottomLeft.x, o->GetBox().bottomLeft.y);
	SDL_RenderDrawLine(pRenderer, o->GetBox().bottomLeft.x, o->GetBox().bottomLeft.y, o->GetBox().bottomRight.x, o->GetBox().bottomRight.y);
	SDL_RenderDrawLine(pRenderer, o->GetBox().topRight.x, o->GetBox().topRight.y, o->GetBox().bottomRight.x, o->GetBox().bottomRight.y);
}

void CalculateForce(std::vector<Object*> &objects)
{
	for (auto& object : objects)
	{
		if (object->rb->mass > 0.f)
		{
			// Add the gravity vector to force
			object->rb->force = object->rb->force + Vector2(0.f, G*object->rb->gravityScale*object->rb->mass);
		}
	}
}

void UpdateObjects(std::vector<Object*> &objects)
{
	for (auto& object : objects)
	{
		SDL_SetRenderDrawColor(pRenderer, object->GetColor().r, object->GetColor().g, object->GetColor().b, object->GetColor().a);
		
		// If the objects mass is set to 0, consider it as infinite mass and don't update
		if (object->rb->mass > 0.f)
		{
			// Timestep is constant in this simulation, but make a variable to make the code more readable
			float dt = (CONST_PHYSICS_DELAY / 1000);
			object->rb->velocity = object->rb->velocity + object->rb->force * (1.0f / object->rb->mass + G) * dt;
			object->rb->position = object->rb->position + object->rb->velocity * dt;
			object->rb->angularAcceleration = object->rb->torque / object->rb->momentOfInertia;
			object->rb->angularVelocity = object->rb->angularVelocity + object->rb->angularAcceleration * dt;
			object->rb->orientation +=  object->rb->angularVelocity * dt;

#if defined DEBUG_EXTENSIVE
			std::cout << "Objects force: " << object->rb->force.x << ", " << object->rb->force.y << std::endl;
			std::cout << "Objects velocity: " << object->rb->velocity.x << ", " << object->rb->velocity.y << std::endl;
			std::cout << "Objects position: " << object->rb->position.x << ", " << object->rb->position.y << std::endl;
			std::cout << "Objects torque: " << object->rb->torque << std::endl;
			std::cout << "Objects angularVelocity: " << object->rb->angularVelocity << std::endl;
			std::cout << "Objects angle: " << object->rb->orientation << "\n" << std::endl;
#endif
		}
		else // This section applies to all objects that have infinite mass
		{
			// Make them rotate to the left constantly
			object->rb->orientation -= 0.008f;
		}
		// Update position and rotation
		object->UpdateBoxPos();
		object->UpdateRotation();
		// Draw the object
		DrawObject(object);
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
	std::uniform_real_distribution<float> pos_dist_x(RECT_MIN_X, RECT_MAX_X);
	std::uniform_real_distribution<float> mass_dist(MASS_MIN, MASS_MAX);
	std::uniform_real_distribution<float> pos_dist_y(RECT_MIN_Y, RECT_MAX_Y);
	std::uniform_int_distribution<int> color(0, 255);


	// Get random values from the generator
	int size_rand = size_dist(generator);
	float pos_rand_x = pos_dist_x(generator);
	float pos_rand_y = pos_dist_y(generator);
	pos_rand_y = -pos_rand_y;
	float mass_rand = mass_dist(generator);
	int color_1_rand = color(generator);
	int color_2_rand = color(generator);
	int color_3_rand = color(generator);

	// Set variables for the object
	RGB objectColor = RGB(color_1_rand, color_2_rand, color_3_rand, 255);
	Vector2 pos = Vector2(pos_rand_x, pos_rand_y);
	Vector2 vel = Vector2(0, 0);
	float accel = 0.f;
	float orientation = 0.f;
	float angVel = 0.f;
	float angAccel = 0.f;
	float torq = 0.f;
	float restitution = 0.6f;
	float inertia = 0.f;
	float gravityScale = 0.0001f;

	// Create object
	Rigidbody* rb = new Rigidbody(pos, vel, accel, orientation, angVel, angAccel, torq, mass_rand, restitution, inertia, gravityScale);
	Object* object = new Object(rb, objectColor, Vector2(size_rand, size_rand));
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
				if (CheckSatCollision(objects[i], objects[j]))
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
									SDL_RENDERER_ACCELERATED);



	std::vector<Object*> objects;

	// Set background color 
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(pRenderer);
	SDL_RenderPresent(pRenderer);

	SDL_Event e;
	bool quit = false;

	// Create the four infinite mass objects on our screen
	for (size_t i = 0; i < 4; i++)
	{
		RGB color = RGB(255, 255, 255, 255);
		Vector2 pos = Vector2((500 * i)+250, 900);
		Rigidbody* rb = new Rigidbody(pos, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
		Object* object = new Object(rb, color, Vector2(200, 200));
		objects.push_back(object);
	}

	int counter = 0;
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

		// Automatically create a new object every 200 update cycles
		if (counter % 200 == 0)
		{
			CreateObject(objects);
		}

		// Set background color 
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		CalculateForce(objects);
		CollisionCheck(objects);
		UpdateObjects(objects);
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