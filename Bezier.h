#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glut.h>
#include "Camera.h"
#include "main.h"

class Bezier
{
public:
	Vector4 p0, p1, p2, p3,
		p4, p5, p6, p7,
		p8, p9, p10, p11,
		p12, p13, p14, p15;

	Bezier();
	Bezier(float, float, float);
	Bezier(Vector3, Vector3, Vector3, Vector3);
	Bezier(Vector3, Vector3, Vector3, float);
	Bezier(Vector4, Vector4, Vector4, Vector4,
		Vector4, Vector4, Vector4, Vector4, float);
	void setCamPos(Matrix4);
	void tessellate(float, float, float);
	Vector4 lip(float, float);
	void drawSphere(Vector4);
	void moveCam(Vector4);
	void drawCP();
};
#endif