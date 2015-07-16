#pragma once

#include "../common/GridLoop.hpp"
#include "SlitherlinkMethod.hpp"

namespace Penciloid
{
class SlitherlinkProblem;

class SlitherlinkField : public GridLoop<SlitherlinkField>
{
public:
	static const int HINT_NONE = -1;

	SlitherlinkField();
	~SlitherlinkField();

	void Init(int height_t, int width_t, bool enable_assumption_table = false);
	void Init(SlitherlinkProblem &prob, bool enable_assumption_table = false);
	void Init(SlitherlinkField &field);

	void SetMethod(SlitherlinkMethod &method_t) { method = method_t; GridLoop<SlitherlinkField>::SetMethod(method.grid_loop_method);  }
	SlitherlinkMethod GetMethod() { return method; }

	void CheckCellSpecific(int y, int x);
	void SegmentDetermined(int y, int x);

	inline int GetHint(int y, int x) { return hints[CellId(y, x)]; }
	inline int GetHintSafe(int y, int x) { return (0 <= y && y < GetHeight() && 0 <= x && x < GetWidth()) ? GetHint(y, x) : HINT_NONE; }
	int SetHint(int y, int x, int hint);
	int Assume();

	void Debug();

private:
	void CheckDiagonalChain(int y, int x);
	void CheckTheorem(int y, int x);
	inline int CellId(int y, int x) { return y * GetWidth() + x; }
	inline void SetAssumptionCandidate(int y, int x) { if (IsProperCoordinate(y, x)) assumption_table[y * (2 * GetWidth() + 1) + x] = true; }

	int *hints; bool *assumption_table;
	SlitherlinkMethod method;
};

}
