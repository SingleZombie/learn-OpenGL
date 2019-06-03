#include "MyVertex.h"
#include <iostream>
#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>

using namespace MyVertex;

VertexProperty::VertexProperty() : _ptr(nullptr), _indice()
{

}

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

void MyVertex::drawVertex(const PropertyArray& properties, UINT properBits, UINT startPos, int count)
{
	int sz = -1;
	for (UINT i = 0; i < TOTAL_INDEX; i++)
	{
		if (properBits & (1 << i))
		{
			assert(properties.size() > i);
			if (sz != -1)
			{
				assert(sz == properties[i].size());
			}
			sz = properties[i].size();
		}
	}

	if (count == -1)
	{
		count = sz;
	}
	else
	{
		count = min(sz, startPos + count);
	}

	for (UINT i = startPos; i < count; i ++)
	{
		const float* pv;

		if (properBits & NORMAL_BIT)
		{
			pv = &properties[NORMAL_INDEX][i];
			glNormal3f(*pv, *(pv + 1), *(pv + 2));
		}
		if (properBits & COLOR_BIT)
		{
			pv = &properties[COLOR_INDEX][i];
			glColor3f(*pv, *(pv + 1), *(pv + 2));
		}
		if (properBits & TEX_COORD2_BIT)
		{
			pv = &properties[TEX_COORD2_INDEX][i];
			glTexCoord2f(*pv, *(pv + 1));
		}
		if (properBits & POSITION_BIT)
		{
			pv = &properties[POSITION_INDEX][i];
			glVertex3f(*pv, *(pv + 1), *(pv + 2));
		}
	}

}