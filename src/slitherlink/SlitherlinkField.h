#pragma once

#include "../common/GridLoop.hpp"
#include "SlitherlinkMethod.hpp"

namespace Penciloid
{
class SlitherlinkProblem;

class SlitherlinkField : public GridLoop<SlitherlinkField>
{
public:
	static const int CLUE_NONE = -1;

	SlitherlinkField();
	~SlitherlinkField();

	void Init(int height_t, int width_t, bool enable_assumption_table = false);
	void Init(SlitherlinkProblem &prob, bool enable_assumption_table = false);
	void Init(SlitherlinkField &field);

	void SetMethod(SlitherlinkMethod &method_t) { method = method_t; GridLoop<SlitherlinkField>::SetMethod(method.grid_loop_method);  }
	SlitherlinkMethod GetMethod() { return method; }

	void CheckCellSpecific(int y, int x);
	void SegmentDetermined(int y, int x);

	inline int GetClue(int y, int x) { return clues[CellId(y, x)]; }
	inline int GetClueSafe(int y, int x) { return (0 <= y && y < GetHeight() && 0 <= x && x < GetWidth()) ? GetClue(y, x) : CLUE_NONE; }
	int SetClue(int y, int x, int clue);
	int Assume();

	void Debug();

private:
	void CheckDiagonalChain(int y, int x);
	void CheckTheorem(int y, int x);
	inline int CellId(int y, int x) { return y * GetWidth() + x; }
	inline void SetAssumptionCandidate(int y, int x) { if (IsProperCoordinate(y, x)) assumption_table[y * (2 * GetWidth() + 1) + x] = true; }

	int *clues; bool *assumption_table;
	SlitherlinkMethod method;
};

}
