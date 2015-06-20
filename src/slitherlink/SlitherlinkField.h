#pragma once

#include "../common/GridLoop.hpp"
#include "SlitherlinkMethod.hpp"

namespace Penciloid
{
class SlitherlinkProblem;

class SlitherlinkField
{
public:
	enum {
		LOOP_UNDECIDED = 0,
		LOOP_LINE = 1,
		LOOP_BLANK = 2
	};
	static const int HINT_NONE = -1;

	SlitherlinkField();
	~SlitherlinkField();

	void Init(int height_t, int width_t);
	void Init(SlitherlinkProblem &prob);
	void Init(SlitherlinkField &field);

	void SetMethod(SlitherlinkMethod &method_t) { method = method_t; grid.SetMethod(method.grid_loop_method);  }
	SlitherlinkMethod GetMethod() { return method; }

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetStatus() { return grid.GetStatus(); }
	inline int GetSegmentStyle(int y, int x) { return grid.GetSegmentStyle(y, x); }
	inline int GetProgress() const { return grid.GetProgress(); }

	inline int GetHint(int y, int x) { return hints[CellId(y, x)]; }
	inline int GetHintSafe(int y, int x) { return grid.CheckCellRange(y, x) ? GetHint(y, x) : HINT_NONE; }
	int SetHint(int y, int x, int hint);
	inline int CheckAll() { return grid.CheckAll(); }
	int Assume();
	inline int CheckInOutRule() { return grid.CheckInOutRule(); }
	inline int CheckConnectability() { return grid.CheckConnectability(); }

	void Debug();
	void Debug2() { grid.Debug2(); }

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
			slither->CheckCell(grid, y, x);
		}
		inline void CheckVertex(GridLoop<SlitherlinkAuxiliarySolver> &grid, int y, int x) {
		}

	private:
		SlitherlinkField *slither;
	};

	void CheckCell(GridLoop<SlitherlinkAuxiliarySolver> &grid, int y, int x);
	void CheckDiagonalChain(GridLoop<SlitherlinkAuxiliarySolver> &grid, int y, int x);
	void CheckTheorem(int y, int x);
	inline int CellId(int y, int x) { return y * width + x; }

	GridLoop<SlitherlinkAuxiliarySolver> grid;
	SlitherlinkAuxiliarySolver auxiliary;

	int height, width;
	int field_status;
	int *hints;

	SlitherlinkMethod method;
};

}
