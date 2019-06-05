#ifndef SECTOR_H
#define SECTOR_H

#include <iostream>
#include <string>
#include <vector>

class paintableObject
{
public:
	virtual void draw() = 0;
};

class Vertex : paintableObject
{
	friend std::istream& operator>>(std::istream& is, Vertex& v);
public:
	float x, y, z;
	float tx, ty;
	void draw();
};
class Triangle : paintableObject
{
	friend std::istream& operator>>(std::istream& is, Triangle& tr);
public:
	Vertex vertexs[3];
	void draw();
};

class Sector : paintableObject
{
	friend std::istream& operator>>(std::istream& is, Sector& s);
public:
	Sector() {}
	Sector(const std::string& fileName);
	void draw();
private:
	std::vector<Triangle> _triangles;
};

#endif