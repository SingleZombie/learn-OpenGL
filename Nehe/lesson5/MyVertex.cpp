#include "MyVertex.h"
#include <iostream>
#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>

using namespace MyVertex;

VertexProperty::VertexProperty(float* value, const UINT* indice, UINT len, UINT step) :
	_ptr(value),
	_indice(len)
{
	for (int i = 0; i < len; i++)
	{
		_indice[i] = indice[i] * step;
	}
}

const float& VertexProperty::operator[](UINT index) const
{
	assert(index < size());

	return _ptr[_indice[index]];
}

float& VertexProperty::operator[](UINT index)
{
	assert(index < size());

	return _ptr[_indice[index]];
}

UINT VertexProperty::size() const
{
	return _indice.size();
}

void MyVertex::drawVertex(const PropertyArray& properties, UINT properBits)
{
	UINT sz = 0;
	if (properBits & POSITION_BIT)
	{
		assert(properties.size() > POSITION_INDEX);
		sz = properties[POSITION_INDEX].size();
	}
	if (properBits & COLOR_BIT)
	{
		assert(properties.size() > COLOR_INDEX);
		sz = properties[COLOR_INDEX].size();
	}

	//static bool upd = false;


	for (UINT i = 0; i < sz; i ++)
	{
		const float* pv;

		if (properBits & COLOR_BIT)
		{
			pv = &properties[COLOR_INDEX][i];
			glColor3f(*pv, *(pv + 1), *(pv + 2));
		}
		if (properBits & POSITION_BIT)
		{
			pv = &properties[POSITION_INDEX][i];
			glVertex3f(*pv, *(pv + 1), *(pv + 2));
		}
	}

	//upd = true;
}