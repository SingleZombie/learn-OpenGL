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

	class VertexProperty
	{
	public:
		VertexProperty();
		VertexProperty(float* value, const UINT* indice, UINT len, UINT step = 3, bool copyData = false);
		~VertexProperty();
		const float& operator[](UINT index) const;
		float& operator[](UINT index);
		UINT size() const;
	private:
		float* _ptr;
		bool _ptrNeedRelease;
		std::vector<UINT> _indice;
	};

	using PropertyArray = std::vector<VertexProperty>;

	void drawVertex(const PropertyArray& properties, UINT properBits, UINT startPos = 0, int count = -1);
};

#endif