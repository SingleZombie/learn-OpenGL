#include "Sector.h"
#include <fstream>
#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>

std::istream& operator>>(std::istream& is, Vertex& v)
{
	return is >> v.x >> v.y >> v.z >> v.tx >> v.ty;
}

void Vertex::draw()
{
	glTexCoord2f(tx, ty);
	glVertex3f(x, y, z);
}

std::istream& operator>>(std::istream& is, Triangle& tr)
{
	for (int i = 0; i < 3; i++)
	{
		is >> tr.vertexs[i];
	}
	return is;
}

void Triangle::draw()
{
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 3; i++)
	{
		vertexs[i].draw();
	}
	glEnd();
}

std::istream& operator>>(std::istream& is, Sector& s)
{
	Triangle tr;
	while (is >> tr)
	{
		s._triangles.push_back(tr);
	}
	return is;
}

Sector::Sector(const std::string& fileName)
{
	std::ifstream fin(fileName);

	fin >> *this;
}

void Sector::draw()
{
	for (auto tr : _triangles)
	{
		tr.draw();
	}
}