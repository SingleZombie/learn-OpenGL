// MyVertex.h
// Provide some functions to store vertex message and draw vertice
//-------------------------
#ifndef MY_VERTEX_H
#define MY_VERTEX_H

#include <cassert>
#include <vector>

namespace MyVertex
{
	using UINT = unsigned int;

	const UINT POSITION_INDEX		= 0;
	const UINT COLOR_INDEX			= 1;
	const UINT TEX_COORD2_INDEX		= 2;
	const UINT NORMAL_INDEX			= 3;
	const UINT TOTAL_INDEX			= 4;

	const UINT POSITION_BIT			= 1 << POSITION_INDEX;
	const UINT COLOR_BIT			= 1 << COLOR_INDEX;
	const UINT TEX_COORD2_BIT		= 1 << TEX_COORD2_INDEX;
	const UINT NORMAL_BIT			= 1 << NORMAL_INDEX;

	// VertexProperty load data from a float array(float vector array) and a indice array.
	// The indice array specifies the indice of vertex in the float vector array.
	// In this way we can reuse the vertex data and make selecting vertice easier.
	class VertexProperty
	{
	public:
		VertexProperty();
		VertexProperty(const VertexProperty& p);
		VertexProperty(float* value, const UINT* indice, UINT len, UINT vectorLen = 3, bool copyData = false);
		~VertexProperty();
		const float& operator[](UINT index) const;
		float& operator[](UINT index);
		UINT size() const;
	private:
		float* _ptr;
		bool _ptrNeedRelease;
		std::vector<UINT> _indice;
	};

	// You must assure the length of each VertexProperty is equal
	class PropertyArray
	{
	public:
		PropertyArray();
		void setProperty(UINT PropertyIndex, const VertexProperty& vertexProperty);
		void draw(UINT propertyBits, UINT startPos = 0, int count = -1) const;
	private:
		std::vector<VertexProperty> _arr;
	};
};

#endif