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

	void Init(int height_t, int width_t);
	void Init(SlitherlinkProblem &prob);
	void Init(SlitherlinkField &field);

	void SetMethod(SlitherlinkMethod &method_t) { method = method_t; GridLoop<SlitherlinkField>::SetMethod(method.grid_loop_method);  }
	SlitherlinkMethod GetMethod() { return method; }

	void CheckCellSpecific(int y, int x);

	inline int GetHint(int y, int x) { return hints[CellId(y, x)]; }
	inline int GetHintSafe(int y, int x) { return (0 <= y && y < GetHeight() && 0 <= x && x < GetWidth()) ? GetHint(y, x) : HINT_NONE; }
	int SetHint(int y, int x, int hint);
	int Assume();

	void Debug();

private:
	void CheckDiagonalChain(int y, int x);
	void CheckTheorem(int y, int x);
	inline int CellId(int y, int x) { return y * GetWidth() + x; }

	int *hints;
	SlitherlinkMethod method;
};

}
