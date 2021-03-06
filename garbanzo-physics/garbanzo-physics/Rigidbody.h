#ifndef _RIGIDBODY_HEADER_
#define _RIGIDBODY_HEADER_

#include "Vector2.h"

class Rigidbody
{
public:
	// Linear components
	Vector2 position;
	Vector2 velocity;
	float acceleration;

	// Angular components
	float orientation;
	float angularVelocity;
	float angularAcceleration;
	float torque;

	// General components
	Vector2 force;
	float gravityScale;
	float mass;
	float restitution;
	float momentOfInertia;

	Rigidbody(Vector2 pos, Vector2 vel, float a, float o, float angVel, float angAccl, float t, float m, float e, float i, float gs);
	~Rigidbody();



};


#endif
