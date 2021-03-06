/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef HINGE_MOTOR_H
#define HINGE_MOTOR_H

class HingeMotor : public Test
{
public:
	HingeMotor()
	{
		{
			b3BodyDef bd;
			b3Body* ground = m_world.CreateBody(bd);

			b3HullShape shape;
			shape.m_hull = &m_groundHull;

			b3ShapeDef sd;
			sd.shape = &shape;
			
			ground->CreateShape(sd);
		}

		b3Body* hinge, *door;

		{
			b3BodyDef bd;
			bd.type = b3BodyType::e_staticBody;
			bd.position.Set(0.0f, 7.0f, 0.0f);
			hinge = m_world.CreateBody(bd);

			b3CapsuleShape shape;
			shape.m_centers[0].Set(0.0f, 0.0f, -4.0f);
			shape.m_centers[1].Set(0.0f, 0.0f, 4.0f);
			shape.m_radius = 0.5f;
			
			b3ShapeDef sd;
			sd.shape = &shape;
			sd.density = 1.0f;

			hinge->CreateShape(sd);
			
			m_body = hinge;
		}

		{
			b3BodyDef bd;
			bd.type = b3BodyType::e_dynamicBody;
			bd.position.Set(2.0f, 7.0f, 0.0f);

			door = m_world.CreateBody(bd);

			m_doorBox.Set(1.0f, 0.5f, 4.0f);

			b3HullShape hull;
			hull.m_hull = &m_doorBox;

			b3ShapeDef sdef;
			sdef.shape = &hull;
			sdef.density = 1.0f;
			
			door->CreateShape(sdef);
		}
		
		{
			b3Vec3 axis(0.0f, 0.0f, 1.0f);
			b3Vec3 anchor(0.0f, 7.0f, 0.0f);
			
			b3RevoluteJointDef jd;
			jd.Initialize(hinge, door, axis, anchor, 0.0f, B3_PI);
			jd.motorSpeed = B3_PI;
			jd.maxMotorTorque = door->GetMass() * 10000.0f;
			jd.enableMotor = true;

			m_rj = (b3RevoluteJoint*)m_world.CreateJoint(jd);
		}

		// Invalidate the orientation
		b3Vec3 axis(1.0f, 0.0f, 0.0f);
		float32 angle = B3_PI;
		door->SetTransform(door->GetPosition(), axis, angle);		
	}

	void KeyDown(int button)
	{
		if (button == GLFW_KEY_M)
		{
			m_rj->SetEnableMotor(!m_rj->IsMotorEnabled());
		}

		if (button == GLFW_KEY_L)
		{
			m_rj->SetEnableLimit(!m_rj->IsLimitEnabled());
		}
		
		if (button == GLFW_KEY_D)
		{
			m_body->SetType(e_dynamicBody);
		}

		if (button == GLFW_KEY_S)
		{
			m_body->SetType(e_staticBody);
		}

		if (button == GLFW_KEY_K)
		{
			m_body->SetType(e_kinematicBody);
		}
	}

	static Test* Create()
	{
		return new HingeMotor();
	}

	b3BoxHull m_doorBox;
	b3Body* m_body;
	b3RevoluteJoint* m_rj;
};

#endif
