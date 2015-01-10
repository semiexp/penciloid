#pragma once

#include "../common/GridLoop.hpp"

namespace Penciloid
{
class SlitherlinkField
{
public:
	enum {
		LOOP_UNDECIDED = 0,
		LOOP_LINE = 1,
		LOOP_BLANK = 2
	};
	const int HINT_NONE = -1;

	SlitherlinkField();
	~SlitherlinkField();

	void Init(int height_t, int width_t);

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetStatus() { return grid.GetStatus(); }
	inline int GetSegmentStyle(int y, int x) { return grid.GetSegmentStyle(y, x); }

	inline int GetHint(int y, int x) { return hints[CellId(y, x)]; }
	int SetHint(int y, int x, int hint);

	void Debug();

private:

	class SlitherlinkAuxiliarySolver
	{
	public:
		SlitherlinkAuxiliarySolver()
		{
			slither = nullptr;
		}

		void Init(SlitherlinkField *slither_t) { slither = slither_t; }

		inline void CheckCell(GridLoop<SlitherlinkAuxiliarySolver> &grid, int y, int x) {
			slither->CheckCell(y, x);
		}
		inline void CheckVertex(GridLoop<SlitherlinkAuxiliarySolver> &grid, int y, int x) {}

	private:
		SlitherlinkField *slither;
	};

	void CheckCell(int y, int x);
	void CheckTheorem(int y, int x);
	inline int CellId(int y, int x) { return y * width + x; }

	GridLoop<SlitherlinkAuxiliarySolver> grid;
	SlitherlinkAuxiliarySolver auxiliary;

	int height, width;
	int field_status;
	int *hints;
};

}