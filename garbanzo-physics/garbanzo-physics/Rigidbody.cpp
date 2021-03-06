#include "Rigidbody.h"



Rigidbody::Rigidbody(	Vector2 pos, 
						Vector2 vel,
						float a,
						float o,
						float angVel,
						float angAccl,
						float t,
						float m,
						float e,
						float i,
						float gs)
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
	gravityScale = gs;
	force = Vector2(0, 0);
}


Rigidbody::~Rigidbody()
{
}
