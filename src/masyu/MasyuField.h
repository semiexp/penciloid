#pragma once

#include "../common/GridLoop.hpp"

namespace Penciloid
{
class MasyuField
{
public:
	enum {
		LOOP_UNDECIDED = 0,
		LOOP_LINE = 1,
		LOOP_BLANK = 2
	};

	enum {
		HINT_NONE = 0,
		HINT_BLACK = 1,
		HINT_WHITE = 2
	};

	MasyuField();
	~MasyuField();

	void Init(int height_t, int width_t);
	void Init(MasyuField &prob);

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetStatus() { return grid.GetStatus(); }
	inline int GetSegmentStyle(int y, int x) { return grid.GetSegmentStyle(y, x); }

	inline int GetHint(int y, int x) { return hints[VertexId(y, x)]; }
	inline int GetHintSafe(int y, int x) { return grid.CheckVertexRange(y, x) ? GetHint(y, x) : HINT_NONE; }
	int SetHint(int y, int x, int hint);
	inline int CheckAll() { return grid.CheckAll(); }

	void Debug();
	void Debug2() { grid.Debug2(); }

private:

	class MasyuAuxiliarySolver
	{
	public:
		MasyuAuxiliarySolver()
		{
			masyu = nullptr;
		}

		void Init(MasyuField *slither_t) { masyu = slither_t; }

		inline void CheckCell(GridLoop<MasyuAuxiliarySolver> &grid, int y, int x) {
		}
		inline void CheckVertex(GridLoop<MasyuAuxiliarySolver> &grid, int y, int x) {
			masyu->CheckVertex(y, x);
		}

	private:
		MasyuField *masyu;
	};

	void CheckVertex(int y, int x);
	void CheckTheorem(int y, int x);
	inline int VertexId(int y, int x) { return y * width + x; }

	GridLoop<MasyuAuxiliarySolver> grid;
	MasyuAuxiliarySolver auxiliary;

	int height, width;
	int field_status;
	int *hints;
};

}
