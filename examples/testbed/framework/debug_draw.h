/*
* Copyright (c) 2016-2016 Irlan Robson http://www.irlan.net
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

#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <bounce/bounce.h>

struct DrawPoints;
struct DrawLines;
struct DrawTriangles;
struct DrawWire;
struct DrawSolid;

//
struct Ray3
{
	b3Vec3 A() const;
	b3Vec3 B() const;

	b3Vec3 direction;
	b3Vec3 origin;
	float32 fraction;
};

inline b3Vec3 Ray3::A() const
{
	return origin;
}

inline b3Vec3 Ray3::B() const
{
	return origin + fraction * direction;
}

//
class Camera
{
public:
	Camera()
	{
		m_center.SetZero();
		m_q.SetIdentity();
		m_width = 1024.0f;
		m_height = 768.0f;
		m_zNear = 1.0f;
		m_zFar = 1000.0f;
		m_fovy = 0.25f * B3_PI;
		m_zoom = 10.0f;
	}

	b3Mat44 BuildProjectionMatrix() const;
	b3Mat44 BuildViewMatrix() const;
	b3Transform BuildViewTransform() const;
	b3Mat44 BuildWorldMatrix() const;
	b3Transform BuildWorldTransform() const;
	
	b3Vec2 ConvertWorldToScreen(const b3Vec3& pw) const;
	Ray3 ConvertScreenToWorld(const b3Vec2& ps) const;

	float32 m_zoom;
	b3Vec3 m_center;
	b3Quat m_q;
	float32 m_width, m_height;
	float32 m_fovy;
	float32 m_zNear;
	float32 m_zFar;
};

inline b3Mat44 MakeMat44(const b3Transform& T)
{
	return b3Mat44(
		b3Vec4(T.rotation.x.x, T.rotation.x.y, T.rotation.x.z, 0.0f),
		b3Vec4(T.rotation.y.x, T.rotation.y.y, T.rotation.y.z, 0.0f),
		b3Vec4(T.rotation.z.x, T.rotation.z.y, T.rotation.z.z, 0.0f),
		b3Vec4(T.position.x, T.position.y, T.position.z, 1.0f));
}

inline b3Mat44 Camera::BuildProjectionMatrix() const
{
	float32 t = tan(0.5f * m_fovy);
	float32 sy = 1.0f / t;

	float32 aspect = m_width / m_height;
	float32 sx = 1.0f / (aspect * t);

	float32 invRange = 1.0f / (m_zNear - m_zFar);
	float32 sz = invRange * (m_zNear + m_zFar);
	float32 tz = invRange * m_zNear * m_zFar;

	b3Mat44 m;
	m.x = b3Vec4(sx, 0.0f, 0.0f, 0.0f);
	m.y = b3Vec4(0.0f, sy, 0.0f, 0.0f);
	m.z = b3Vec4(0.0f, 0.0f, sz, -1.0f);
	m.w = b3Vec4(0.0f, 0.0f, tz, 0.0f);
	return m;
}

inline b3Transform Camera::BuildWorldTransform() const
{
	b3Transform xf;
	xf.rotation = b3QuatMat33(m_q);
	xf.position = (m_zoom * xf.rotation.z) - m_center;
	return xf;
}

inline b3Mat44 Camera::BuildWorldMatrix() const
{
	b3Transform xf = BuildWorldTransform();
	return MakeMat44(xf);
}

inline b3Transform Camera::BuildViewTransform() const
{
	b3Transform xf;
	xf.rotation = b3QuatMat33(m_q);
	xf.position = (m_zoom * xf.rotation.z) - m_center;
	return b3Inverse(xf);
}

inline b3Mat44 Camera::BuildViewMatrix() const
{
	b3Transform xf = BuildViewTransform();
	return MakeMat44(xf);
}

inline b3Vec2 Camera::ConvertWorldToScreen(const b3Vec3& pw) const
{
	b3Vec2 ps;
	ps.SetZero();
	return ps;
}

inline Ray3 Camera::ConvertScreenToWorld(const b3Vec2& ps) const
{
	// Essential Math, page 250.
	float32 t = tan(0.5f * m_fovy);
	float32 aspect = m_width / m_height;

	b3Vec3 pv;
	pv.x = 2.0f * aspect * ps.x / m_width - aspect;
	pv.y = -2.0f * ps.y / m_height + 1.0f;
	pv.z = -1.0f / t;

	b3Transform xf = BuildWorldTransform();
	b3Vec3 pw = xf * pv;

	Ray3 rw;
	rw.direction = b3Normalize(pw - xf.position);
	rw.origin = xf.position;
	rw.fraction = m_zFar;
	return rw;
}

//
class DebugDraw : public b3Draw
{
public:
	DebugDraw();
	~DebugDraw();

	void DrawPoint(const b3Vec3& p, float32 size, const b3Color& color);

	void DrawSegment(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color);
	
	void DrawTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color);

	void DrawSolidTriangle(const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color);

	void DrawSolidTriangle(const b3Vec3& normal, const b3Vec3& p1, const b3Color& color1, const b3Vec3& p2, const b3Color& color2, const b3Vec3& p3, const b3Color& color3);
	
	void DrawPolygon(const b3Vec3* vertices, u32 count, const b3Color& color);
	
	void DrawSolidPolygon(const b3Vec3& normal, const b3Vec3* vertices, u32 count, const b3Color& color);

	void DrawCircle(const b3Vec3& normal, const b3Vec3& center, float32 radius, const b3Color& color);

	void DrawSolidCircle(const b3Vec3& normal, const b3Vec3& center, float32 radius, const b3Color& color);

	void DrawSphere(const b3Vec3& center, float32 radius, const b3Color& color);
	
	void DrawSolidSphere(const b3Vec3& center, float32 radius, const b3Color& color);

	void DrawCapsule(const b3Vec3& p1, const b3Vec3& p2, float32 radius, const b3Color& color);

	void DrawSolidCapsule(const b3Vec3& p1, const b3Vec3& p2, float32 radius, const b3Color& color);

	void DrawAABB(const b3AABB3& aabb, const b3Color& color);

	void DrawTransform(const b3Transform& xf);

	//
	void DrawString(const b3Color& color, const char* string, ...);
	
	void DrawSphere(const b3SphereShape* s, const b3Color& c, const b3Transform& xf);

	void DrawCapsule(const b3CapsuleShape* s, const b3Color& c, const b3Transform& xf);
	
	void DrawHull(const b3HullShape* s, const b3Color& c, const b3Transform& xf);
	
	void DrawMesh(const b3MeshShape* s, const b3Color& c, const b3Transform& xf);

	void DrawShape(const b3Shape* s, const b3Color& c, const b3Transform& xf);

	void Draw(const b3World& world);

	void Submit();
private:
	friend struct DrawShapes;

	DrawPoints* m_points;
	DrawLines* m_lines;
	DrawTriangles* m_triangles;
	DrawWire* m_wire;
	DrawSolid* m_solid;
};

extern DebugDraw* g_debugDraw;
extern Camera* g_camera;
extern const char* g_overlayName;

#endif