#ifndef MY_VERTEX_H
#define MY_VERTEX_H

#include <cassert>
#include <vector>

namespace MyVertex
{
	using UINT = unsigned int;

	const UINT POSITION_INDEX = 0;
	const UINT COLOR_INDEX = 1;

	const UINT POSITION_BIT = 1 << POSITION_INDEX;
	const UINT COLOR_BIT = 1<< COLOR_INDEX;

	class VertexProperty
	{
	public:
		VertexProperty(float* value, const UINT* indice, UINT len, UINT step = 3);
		const float& operator[](UINT index) const;
		float& operator[](UINT index);
		UINT size() const;
	private:
		float* _ptr;
		std::vector<UINT> _indice;
	};

	using PropertyArray = std::vector<VertexProperty>;

	void drawVertex(const PropertyArray& properties, UINT properBits);
};

#endif