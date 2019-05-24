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
#define RECT_Y -10
#define MASS_MIN 1
#define MASS_MAX 5

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

Vector2 projectOnAxis(std::vector<Vector2> vertices, const Vector2& axis)
{
	float min = std::numeric_limits<float>::infinity();
	float max = -std::numeric_limits<float>::infinity();
	for (auto& vertex : vertices) 
	{
		float projection = DotProduct(vertex, axis);
		if (projection < min) 
		{ 
			min = projection;
		}
		if (projection > max) 
		{ 
			max = projection;
		}
	}
	return Vector2(min, max);
}

Vector2 project(Vector2 pos, Vector2 axis)
{
	float total = pos.x * axis.x + pos.y * axis.y;
	total = total / (axis.x * axis.x + axis.y * axis.y);
	return Vector2(axis.x * total, axis.y * total);
}

bool CheckSatCollision(Object* first, Object* second)
{
	std::vector<Vector2> firstCorners;
	
	firstCorners.push_back(Vector2(first->GetBox().topLeft.x, first->GetBox().topLeft.y));
	firstCorners.push_back(Vector2(first->GetBox().topRight.x, first->GetBox().topRight.y));
	firstCorners.push_back(Vector2(first->GetBox().bottomRight.x, first->GetBox().bottomRight.y));
	firstCorners.push_back(Vector2(first->GetBox().bottomLeft.x, first->GetBox().bottomLeft.y));


	std::vector<Vector2> secondCorners;
	secondCorners.push_back(Vector2(second->GetBox().topLeft.x, second->GetBox().topLeft.y));
	secondCorners.push_back(Vector2(second->GetBox().topRight.x, second->GetBox().topRight.y));
	secondCorners.push_back(Vector2(second->GetBox().bottomRight.x, second->GetBox().bottomRight.y));
	secondCorners.push_back(Vector2(second->GetBox().bottomLeft.x, second->GetBox().bottomLeft.y));

	std::vector<Vector2> axis;

	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);

	Vector2 firstAxis = first->rb->position - first->GetBox().topNormal;
	Vector2 secondAxis = first->rb->position - first->GetBox().leftNormal;
	Vector2 thirdAxis = second->rb->position - second->GetBox().topNormal;
	Vector2 fourthAxis = second->rb->position - second->GetBox().leftNormal;
	SDL_RenderDrawLine(pRenderer, first->rb->position.x, first->rb->position.y, first->rb->position.x - firstAxis.x, first->rb->position.y - firstAxis.y);
	SDL_RenderDrawLine(pRenderer, first->rb->position.x, first->rb->position.y, first->rb->position.x - secondAxis.x, first->rb->position.y - secondAxis.y);
	SDL_RenderDrawLine(pRenderer, second->rb->position.x, second->rb->position.y, second->rb->position.x - thirdAxis.x, second->rb->position.y - thirdAxis.y);
	SDL_RenderDrawLine(pRenderer, second->rb->position.x, second->rb->position.y, second->rb->position.x - fourthAxis.x, second->rb->position.y - fourthAxis.y);


	axis.push_back(Normalize(Vector2(firstAxis.x, firstAxis.y)));
	axis.push_back(Normalize(Vector2(secondAxis.x, secondAxis.y)));
	axis.push_back(Normalize(Vector2(thirdAxis.x, thirdAxis.y)));
	axis.push_back(Normalize(Vector2(fourthAxis.x, fourthAxis.y)));

	//axis.push_back(Normalize(firstCorners[0] - firstCorners[1]).getNormal());
	//axis.push_back(Normalize(firstCorners[0] - firstCorners[3]).getNormal());
	//axis.push_back(Normalize(secondCorners[0] - secondCorners[1]).getNormal());
	//axis.push_back(Normalize(secondCorners[0] - secondCorners[3]).getNormal());



	for (size_t i = 0; i < axis.size(); i++)
	{
		float aMin = std::numeric_limits<float>::infinity();
		float aMax = -std::numeric_limits<float>::infinity();
		float bMin = std::numeric_limits<float>::infinity();
		float bMax = -std::numeric_limits<float>::infinity();

		for (size_t j = 0; j < firstCorners.size(); j++)
		{
			Vector2 res = project(firstCorners[j], axis[i]);
			float result = DotProduct(axis[i], firstCorners[j]);
			if (result > aMax)
				aMax = result;
			if (result < aMin)
				aMin = result;
		}
		for (size_t j = 0; j < secondCorners.size(); j++)
		{
			Vector2 res = project(secondCorners[j], axis[i]);
			float result = DotProduct(axis[i], secondCorners[j]);
			if (result > bMax)
				bMax = result;
			if (result < bMin)
				bMin = result;
		}

		if (bMin > aMax || bMax < aMin)
		{
			return false;
		}

	}
	return true;
}

//bool CheckSATCollision(Object* first, Object* second)
//{
//	std::vector<Vector2> firstCorners;
//	firstCorners.push_back(first->GetBox().bottomLeft);
//	firstCorners.push_back(first->GetBox().topLeft);
//	firstCorners.push_back(first->GetBox().topRight);
//	firstCorners.push_back(first->GetBox().bottomRight);
//
//
//	std::vector<Vector2> secondCorners;
//	secondCorners.push_back(second->GetBox().bottomLeft);
//	secondCorners.push_back(second->GetBox().topLeft);
//	secondCorners.push_back(second->GetBox().topRight);
//	secondCorners.push_back(second->GetBox().bottomRight);
//
//	// Go through all the corners of first object
//	for (size_t i = 0; i < firstCorners.size(); i++)
//	{
//		Vector2 current = firstCorners[i];
//		Vector2 next = firstCorners[(i + 1) % firstCorners.size()];
//		Vector2 edge = next - current;
//		edge = Normalize(edge);
//
//		// Get the perpendicular of this
//		Vector2 axis = edge.getNormal();
//
//		float aMaxProj = -std::numeric_limits<float>::infinity();
//		float aMinProj = std::numeric_limits<float>::infinity();
//		float bMaxProj = -std::numeric_limits<float>::infinity();
//		float bMinProj = std::numeric_limits<float>::infinity();
//
//
//		for (size_t j = 0; j < firstCorners.size(); j++)
//		{
//			float proj = DotProduct(axis, firstCorners[j]);
//			if (proj < aMinProj)
//				aMinProj = proj;
//			if (proj > aMaxProj)
//				aMaxProj = proj;
//		}
//
//		for (size_t j = 0; j < secondCorners.size(); j++)
//		{
//			float proj = DotProduct(axis, secondCorners[j]);
//			if (proj < bMinProj)
//				bMinProj = proj;
//			if (proj > bMaxProj)
//				bMaxProj = proj;
//		}
//
//		// Check if the intervals of both polygons projected on the axis overlap
//		// If they don't, there exists an axis of separation and the given shapes can't overlap currently
//
//		//std::cout << "SAT values: " << aMaxProj << ", " << aMinProj << ", " << bMaxProj << ", " << bMinProj << std::endl;
//
//		if (aMinProj < bMaxProj && aMaxProj > bMinProj)
//		{
//			return true;
//		}
//	}
//
//	// If the check reaches this point, all of the axis' have been checked but no overlap was found
//	// meaning that the shapes are not intersecting
//	return false;
//}

void ResolveCollision(Object* a, Object* b)
{
	// Linear part
	float aMass;
	float bMass;

	// Check if the mass is 0, if it is the mass is to be considered infinite
	if (a->rb->mass > 0)
		aMass = a->rb->mass;
	else
		aMass = 10000;

	if (b->rb->mass > 0)
		bMass = b->rb->mass;
	else
		bMass = 10000;

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

	Vector2 normal = Normalize(b->rb->position - p);

	SDL_SetRenderDrawColor(pRenderer, 0, 255, 255, 255);
	float contactVel = DotProduct(ab, normal);

	if (contactVel > 0)
		return;

	Vector2 startPoint = a->rb->position;
	Vector2 endPoint = startPoint + normal * 500;
	SDL_RenderDrawLine(pRenderer, startPoint.x, startPoint.y, endPoint.x, endPoint.y);

	float e = fmin(a->rb->restitution, b->rb->restitution);
	float j = -(1.0f + e) * contactVel;
	j /= 1 / aMass + 1 / bMass;

	Vector2 impulse = normal * j;
	a->rb->velocity = a->rb->velocity - impulse * (1 / aMass);
	b->rb->velocity = b->rb->velocity + impulse * (1 / bMass);



	//float e = fmin(a->rb->restitution, b->rb->restitution);
	//a->rb->velocity = a->rb->velocity + (collision_norm * -1.f * e);
	//b->rb->velocity = b->rb->velocity + (collision_norm * e);


	
	// Angular part

	// Rough estimation of the "arm" vectors by getting vector from one object to another and dividing it by two
	Vector2 bToA = (b->rb->position - a->rb->position) / 2;
	Vector2 aToB = (a->rb->position - b->rb->position) / 2;
	a->rb->torque = aToB.x * a->rb->force.y - aToB.y * a->rb->force.x;
	b->rb->torque = bToA.x * b->rb->force.y - bToA.y * b->rb->force.x;
	//std::cout << "Force of object a: " << a->rb->force.x << ", " << a->rb->force.y << std::endl;
	//std::cout << "Force of object b: " << b->rb->force.x << ", " << b->rb->force.y << std::endl;

}
#pragma endregion

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
			object->rb->force = object->rb->force + Vector2(0.f, G*GRAVITY_SCALE*object->rb->mass);
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
			//object->rb->orientation += 0.015f;
			object->rb->velocity = object->rb->velocity + object->rb->force * (1.0f / object->rb->mass + G) * (CONST_PHYSICS_DELAY / 1000);
			object->rb->position = object->rb->position + object->rb->velocity * (CONST_PHYSICS_DELAY / 1000);
			object->rb->angularVelocity += object->rb->torque / object->rb->momentOfInertia * (CONST_PHYSICS_DELAY / 1000);
			object->rb->orientation += object->rb->angularVelocity * (CONST_PHYSICS_DELAY / 1000);

			object->UpdateBoxPos();
			object->UpdateRotation();

#if defined DEBUG_EXTENSIVE
			std::cout << "Objects force: " << object->rb->force.x << ", " << object->rb->force.y << std::endl;
			std::cout << "Objects velocity: " << object->rb->velocity.x << ", " << object->rb->velocity.y << std::endl;
			std::cout << "Objects position: " << object->rb->position.x << ", " << object->rb->position.y << std::endl;
			std::cout << "Objects torque: " << object->rb->torque << std::endl;
			std::cout << "Objects angularVelocity: " << object->rb->angularVelocity << std::endl;
			std::cout << "Objects angle: " << object->rb->orientation << std::endl;
#endif
		}
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
	Vector2 vel = Vector2(0, 0);
	float accel = 0.f;
	float orientation = 0.f;
	float angVel = 0.f;
	float torq = 0.f;
	float restitution = 0.6f;
	float inertia = 0.f;

	Rigidbody* rb = new Rigidbody(pos, vel, accel, orientation, angVel, torq, mass_rand, restitution, inertia);
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

	int counter = 0;

	 //FLOOR
	for (size_t i = 1; i < 27; i++)
	{
		RGB color = RGB(255, 255, 255, 255);
		float widthChange = WIDTH / 25;
		Vector2 pos = Vector2(widthChange * i - widthChange/2, HEIGHT - 250 + 250 / 2);
		Rigidbody* rb = new Rigidbody(pos, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, 0.f, 0.6f, 0.f);
		Object* object = new Object(rb, color, Vector2(widthChange, 250));
		objects.push_back(object);
	}

	//RGB color = RGB(255, 255, 255, 255);
	//Vector2 pos = Vector2(100, 100);
	//Rigidbody* rb = new Rigidbody(pos, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, 0.f, 0.6f, 0.f);
	//Object* object = new Object(rb, color, Vector2(100, 100));
	//objects.push_back(object);

	//RGB color2 = RGB(255, 255, 255, 255);
	//Vector2 pos2 = Vector2(300, 100);
	//Rigidbody* rb2 = new Rigidbody(pos2, Vector2(0, 0), 0.f, 0.f, 0.f, 0.f, 0.f, 0.6f, 0.f);
	//Object* object2 = new Object(rb2, color2, Vector2(100, 100));
	//objects.push_back(object2);
	

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
		if (counter % 50 == 0)
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

		for (auto& object : objects)
		{
			// REMEMBER TO SET FORCE TO ZERO AFTER APPLYING IT ON THIS UPDATE CYCLE
			object->rb->force = Vector2(0.f, 0.f);
		}

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