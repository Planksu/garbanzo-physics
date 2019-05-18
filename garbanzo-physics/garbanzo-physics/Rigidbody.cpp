#include "Rigidbody.h"



Rigidbody::Rigidbody(	Vector2 pos, 
						Vector2 vel,
						float a,
						float o,
						float angVel,
						float t,
						float m,
						float e,
						float i)
{
	position = pos;
	velocity = vel;
	acceleration = a;
	orientation = o;
	angularVelocity = angVel;
	torque = t;
	mass = m;
	restitution = e;
	momentOfInertia = i;
	force = Vector2(0, 0);
}


Rigidbody::~Rigidbody()
{
}
