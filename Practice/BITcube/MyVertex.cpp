#include "MyVertex.h"
#include <iostream>
#include <Windows.h>
#include <gl\glew.h>
#include <gl\glut.h>

using namespace MyVertex;

VertexProperty::VertexProperty() : _ptr(nullptr), _indice()
{

}
VertexProperty::VertexProperty(const VertexProperty& p) : 
	_indice(p._indice),
	_ptrNeedRelease(p._ptrNeedRelease)
{
	if (_ptrNeedRelease)
	{
		UINT len = _indice.size();
		_ptr = new float[len];
		memcpy(_ptr, p._ptr, sizeof(_ptr[0] * len));
	}
}

VertexProperty::VertexProperty(float* value, const UINT* indice, UINT len, UINT vectorLen, bool copyData) :
	_ptr(value),
	_indice(len),
	_ptrNeedRelease(copyData)
{
	for (int i = 0; i < len; i++)
	{
		_indice[i] = indice[i] * vectorLen;
	}
	if (copyData)
	{
		_ptr = new float[len];
		memcpy(_ptr, value, sizeof(value[0]) * len);
	}
}

VertexProperty::~VertexProperty()
{
	if (_ptrNeedRelease)
	{
		delete[] _ptr;
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


PropertyArray::PropertyArray() : _arr(TOTAL_INDEX)
{

}

void PropertyArray::setProperty(UINT PropertyIndex, const VertexProperty& vertexProperty)
{
	assert(PropertyIndex < TOTAL_INDEX);
	_arr[PropertyIndex] = vertexProperty;
}
void PropertyArray::draw(UINT propertyBits, UINT startPos, int count) const
{
	int sz = -1;
	for (UINT i = 0; i < TOTAL_INDEX; i++)
	{
		if (propertyBits & (1 << i))
		{
			assert(_arr.size() > i);
			if (sz != -1)
			{
				assert(sz == _arr[i].size());
			}
			sz = _arr[i].size();
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

	for (UINT i = startPos; i < count; i++)
	{
		const float* pv;

		if (propertyBits & NORMAL_BIT)
		{
			pv = &_arr[NORMAL_INDEX][i];
			glNormal3f(*pv, *(pv + 1), *(pv + 2));
		}
		if (propertyBits & COLOR_BIT)
		{
			pv = &_arr[COLOR_INDEX][i];
			glColor3f(*pv, *(pv + 1), *(pv + 2));
		}
		if (propertyBits & TEX_COORD2_BIT)
		{
			pv = &_arr[TEX_COORD2_INDEX][i];
			glTexCoord2f(*pv, *(pv + 1));
		}
		if (propertyBits & POSITION_BIT)
		{
			pv = &_arr[POSITION_INDEX][i];
			glVertex3f(*pv, *(pv + 1), *(pv + 2));
		}
	}
}