#include "Camera.h"
#include <math.h>
#include <stdio.h>

Camera::Camera(Vector3 ie, Vector3 id, Vector3 iup)
{
	e = ie;
	d = id;
	up = iup;
	c = getInverseCamera();
}

Camera::Camera()
{
	e = Vector3(0, 0, 0);
	d = Vector3(0, 0, 0);
	up = Vector3(0, 0, 0);
	c = getInverseCamera();
}

Matrix4& Camera::getInverseCamera()
{
	Vector3 z = (e - d);
	z.normalize();
	Vector3 x = up.cross(z);
	x.normalize();
	Vector3 y = z.cross(x);

	c.m[0][0] = x.x;
	c.m[0][1] = y.x;
	c.m[0][2] = z.x;
	c.m[0][3] = 0;

	c.m[1][0] = x.y;
	c.m[1][1] = y.y;
	c.m[1][2] = z.y;
	c.m[1][3] = 0;

	c.m[2][0] = x.z;
	c.m[2][1] = y.z;
	c.m[2][2] = z.z;
	c.m[2][3] = 0;

	c.m[3][0] = 0;
	c.m[3][1] = 0;
	c.m[3][2] = 0;
	c.m[3][3] = 1;

	c.transpose();

	Matrix4 inverse;

	inverse.m[0][0] = 1;
	inverse.m[0][1] = 0;
	inverse.m[0][2] = 0;
	inverse.m[0][3] = -e.x;

	inverse.m[1][0] = 0;
	inverse.m[1][1] = 1;
	inverse.m[1][2] = 0;
	inverse.m[1][3] = -e.y;

	inverse.m[2][0] = 0;
	inverse.m[2][1] = 0;
	inverse.m[2][2] = 1;
	inverse.m[2][3] = -e.z;

	inverse.m[3][0] = 0;
	inverse.m[3][1] = 0;
	inverse.m[3][2] = 0;
	inverse.m[3][3] = 1;

	/*Matrix4 translate;
	translate.makeTranslate(0,0, -20);*/

	c = c*inverse;

	return c;
}

void Camera::setFrustum(float a, float r, float nD, float fD)
{
	angle = a;
	ratio = r;
	nearDistance = nD;
	farDistance = fD;

	//cout << "angle: " << angle << endl;
	//cout << "ratio: " << ratio << endl;
	//cout << "nearDistance: " << nearDistance << endl;
	//cout << "farDistance: " << farDistance << endl;

	tang = (float)(tan(radians*angle*0.5));
	nearHeight = nearDistance * tang;
	nearWidth = nearHeight * ratio;
	farHeight = farDistance * tang;
	farWidth = farHeight * ratio;

	//cout << "Tang: " << tang << endl;
	//cout << "nh: " << nearHeight << endl;
	//cout << "nw: " << nearWidth << endl;
	//cout << "fh: " << farHeight << endl;
	//cout << "fw: " << farWidth << endl;
}

void Camera::setCamera(Vector3 ie, Vector3 id, Vector3 iup)
{
	e = ie;
	d = id;
	up = iup;
	c = getInverseCamera();
}

void Camera::Rotate_View(float x, float y, float z)
{
	Vector3 vVector = d - e;

	if (x)
	{
		d.z = (float)(e.z + sin(x)*vVector.y + cos(x)*vVector.z);
		d.y = (float)(e.y + cos(x)*vVector.y - sin(x)*vVector.z);
	}
	if (y)
	{
		d.z = (float)(e.z + sin(y)*vVector.x + cos(y)*vVector.z);
		d.x = (float)(e.x + cos(y)*vVector.x - sin(y)*vVector.z);
	}
	if (z)
	{
		d.x = (float)(e.x + sin(z)*vVector.y + cos(z)*vVector.x);
		d.y = (float)(e.y + cos(z)*vVector.y - sin(z)*vVector.x);
	}
}