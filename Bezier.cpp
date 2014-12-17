#include "Bezier.h"

Bezier::Bezier(Vector4 sp0, Vector4 sp1,
	Vector4 sp4, Vector4 sp5,
	Vector4 sp8, Vector4 sp9,
	Vector4 sp12, Vector4 sp13, float angle)
{
	p0 = sp0;
	p1 = sp1;
	p4 = sp4;
	p5 = sp5;
	p8 = sp8;
	p9 = sp9;
	p12 = sp12;
	p13 = sp13;

	float widthx = p1.x - p0.x;
	//float heightz = p4.z - p0.z;

	/*float s = widthx - heightz;
	float a = widthx + heightz;*/
	
	//p2, p3
	p2.x = p1.x + widthx;
	p2.z = p1.z;
	p3.x = p2.x + widthx;
	p3.z = p2.z;
	//p3.y = cos(a*angle);

	p6.x = p5.x + widthx;
	p6.z = p5.z;
	p7.x = p6.x + widthx;
	p7.z = p6.z;
	//p7.y = cos(s*angle)/50;

	p10.x = p9.x + widthx;
	p10.z = p9.z;
	p11.x = p10.x + widthx;
	p11.z = p10.z;
	//p11.y = cos(a*angle);

	p14.x = p13.x + widthx;
	p14.z = p13.z;
	p15.x = p14.x + widthx;
	p15.z = p14.z;
	//p15.y = cos(s*angle);
}

Bezier::Bezier(float u, float v, float angle)
{
	float q = u / 3; //q is horizontal
	float r = v / 3; //r is vertical

	float s = r - q;
	float a = r + q;

	//p0-p3
	p0.x = 0 * q;
	p1.x = 1 * q;
	p2.x = 2 * q;
	p3.x = 3 * q;

	p0.z = 0 * r;
	p1.z = 0 * r;
	p2.z = 0 * r;
	p3.z = 0 * r;

	p1.y = sin(s*angle);
	//p3.y = cos(a*angle);

	//p4-p7
	p4.x = 0 * q;
	p5.x = 1 * q;
	p6.x = 2 * q;
	p7.x = 3 * q;

	p4.z = 1 * r;
	p5.z = 1 * r;
	p6.z = 1 * r;
	p7.z = 1 * r;

	p5.y = sin(a*angle) / 50;
	p7.y = cos(s*angle) / 50;

	//p8-p11
	p8.x = 0 * q;
	p9.x = 1 * q;
	p10.x = 2 * q;
	p11.x = 3 * q;

	p8.z = 2 * r;
	p9.z = 2 * r;
	p10.z = 2 * r;
	p11.z = 2 * r;

	p9.y = sin(s*angle);
	//p11.y = cos(a*angle);

	//p12-p15
	p12.x = 0 * q;
	p13.x = 1 * q;
	p14.x = 2 * q;
	p15.x = 3 * q;

	p12.z = 3 * r;
	p13.z = 3 * r;
	p14.z = 3 * r;
	p15.z = 3 * r;

	p13.y = sin(a*angle);
	p15.y = cos(s*angle);
}

Bezier::Bezier()
{

}

Bezier::Bezier(Vector3 ip0,Vector3 ip1,Vector3 ip2,Vector3 ip3)
{
	p0.x = ip0.x;
	p0.y = ip0.y;
	p0.z = ip0.z;

	p1.x = ip1.x;
	p1.y = ip1.y;
	p1.z = ip1.z;

	p2.x = ip2.x;
	p2.y = ip2.y;
	p2.z = ip2.z;

	p3.x = ip3.x;
	p3.y = ip3.y;
	p3.z = ip3.z;

	drawCP();
}

void Bezier::drawCP()
{
	Matrix4 tc;
	Matrix4 t;

	if (cp == 1)
	{
		t.makeTranslate(p1.x, p1.y, p1.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.2, 20, 20);

		t.makeTranslate(p2.x, p2.y, p2.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.2, 20, 20);
	}
}

long factorial(long n)
{
	long result = 1;

	//If n is 0, by definition 0! is equal to 1
	if (n <= 0)
		return result;

	//Calculate the factorial, n * n-1 * n-2 * ... * 1
	for (long i = n; i > 0; --i)
	{
		result *= i;
	}

	return result;
}

long combo(long n, long i)
{
	//C(n, i) = n! / ((n-1)! * i!)
	return factorial(n) / (factorial(n - i) * factorial(i));
}

double bernstizzlesCoeff(long n, long i, double t)
{
	//long n: is the degree of our curve, in the case of a cubic curve it is 3
	//long i: the index of the Bernstein coefficient and the control point
	//double t: is the time we are evaluating at

	//Calculate the Bernstein coefficient
	return combo(n, i) * pow(1.0 - t, n - i) * pow(t, i);
}

void Bezier::tessellate(float t1, float t2, float delta)
{
	//q0
	float time = 0.00001;
	//t1 = sin(3.14*t1);
	//t2 = sin(3.14*t2);
	Matrix4 mp0(p0.x, p1.x, p2.x, p3.x,
		p0.y, p1.y, p2.y, p3.y,
		p0.z, p1.z, p2.z, p3.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 C(bernstizzlesCoeff(3, 0, t1),
		bernstizzlesCoeff(3, 1, t1),
		bernstizzlesCoeff(3, 2, t1),
		bernstizzlesCoeff(3, 3, t1));

	Vector4 Cr(bernstizzlesCoeff(3, 0, t1 + time),
		bernstizzlesCoeff(3, 1, t1 + time),
		bernstizzlesCoeff(3, 2, t1 + time),
		bernstizzlesCoeff(3, 3, t1 + time));

	Vector4 q0 = mp0 * C;
	q0.w = 1.0;

	Vector4 r0 = mp0 * Cr;
	r0.w = 1.0;

	//q1
	Matrix4 mp1(p4.x, p5.x, p6.x, p7.x,
		p4.y, p5.y, p6.y, p7.y,
		p4.z, p5.z, p6.z, p7.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 q1 = mp1 * C;
	q1.w = 1.0;

	Vector4 r1 = mp1 * Cr;
	r1.w = 1.0;

	//q2
	Matrix4 mp2(p8.x, p9.x, p10.x, p11.x,
		p8.y, p9.y, p10.y, p11.y,
		p8.z, p9.z, p10.z, p11.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 q2 = mp2 * C;
	q2.w = 1.0;

	Vector4 r2 = mp2 * Cr;
	r2.w = 1.0;

	//q3
	Matrix4 mp3(p12.x, p13.x, p14.x, p15.x,
		p12.y, p13.y, p14.y, p15.y,
		p12.z, p13.z, p14.z, p15.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 q3 = mp3 * C;
	q3.w = 1.0;

	Vector4 r3 = mp3 * Cr;
	r3.w = 1.0;

	//t2

	Matrix4 mq(q0.x, q1.x, q2.x, q3.x,
		q0.y, q1.y, q2.y, q3.y,
		q0.z, q1.z, q2.z, q3.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 C2(bernstizzlesCoeff(3, 0, t2),
		bernstizzlesCoeff(3, 1, t2),
		bernstizzlesCoeff(3, 2, t2),
		bernstizzlesCoeff(3, 3, t2));

	Vector4 Cr2(bernstizzlesCoeff(3, 0, t2 + time),
		bernstizzlesCoeff(3, 1, t2 + time),
		bernstizzlesCoeff(3, 2, t2 + time),
		bernstizzlesCoeff(3, 3, t2 + time));

	Vector4 x = mq * C2;
	x.w = 1.0;

	Vector4 x2 = mq * Cr2;
	x2.w = 1.0;

	Vector4 tanU = x2 - x;

	Matrix4 mr(r0.x, r1.x, r2.x, r3.x,
		r0.y, r1.y, r2.y, r3.y,
		r0.z, r1.z, r2.z, r3.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 v = mr * C2;
	v.w = 1.0;

	Vector4 tanV = v - x;

	Vector3 tU = Vector3(tanU.x, tanU.y, tanU.z);
	Vector3 tV = Vector3(tanV.x, tanV.y, tanV.z);

	tU.normalize();
	tV.normalize();

	Vector3 norm = tU.cross(tV);
	norm.normalize();

	glBegin(GL_QUADS);

	glNormal3d(norm.x, norm.y, norm.z);

	glVertex3d(x.x, x.y, x.z);
	glVertex3d(x.x + delta, x.y, x.z);
	glVertex3d(x.x + delta, x.y, x.z + delta);
	glVertex3d(x.x, x.y, x.z + delta);
	glEnd();

}

Vector4 Bezier::lip(float t, float delta)
{
	Matrix4 mp0(p0.x, p1.x, p2.x, p3.x,
		p0.y, p1.y, p2.y, p3.y,
		p0.z, p1.z, p2.z, p3.z,
		0.0, 0.0, 0.0, 0.0);

	Vector4 C(bernstizzlesCoeff(3, 0, t),
		bernstizzlesCoeff(3, 1, t),
		bernstizzlesCoeff(3, 2, t),
		bernstizzlesCoeff(3, 3, t));

	Vector4 x = mp0 * C;
	x.w = 1.0;
	return x;
}

void Bezier::drawSphere(Vector4 p)
{
	glMatrixMode(GL_MODELVIEW);

	Matrix4 test = objCamera.c;

	//test.print("fuck");

	Matrix4 my;
	my.identity();
	my.m[0][3] = p.x;
	my.m[1][3] = p.y;
	my.m[2][3] = p.z;
	//my.print("fuckyou");

	my = my*test;
	my.transpose();

	glLoadMatrixd(my.getPointer());
	glColor3f(1, 0, 1);
	glutWireSphere(.1, 20, 20);
}

//Camera objCamera;

void Bezier::moveCam(Vector4 p)
{
	//objCamera = Camera(Vector3(p.x, p.y, p.z), Vector3(0, 0, 0), Vector3(0, 1, 0));
}