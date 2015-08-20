#pragma once

#include "SlitherlinkMethod.hpp"

namespace Penciloid
{
class SlitherlinkProblemConstraint
{
public:
	SlitherlinkProblemConstraint() : height(-1), width(-1), field(nullptr) {};
	~SlitherlinkProblemConstraint() {
		if (field) delete[] field;
	}

	void Init(int height_t, int width_t);
	void Init(int height_t, int width_t, int *clue_t, bool enable_clue_restriction = false);
	void Init(int height_t, int width_t, char *clue_t[], bool enable_clue_restriction = false);

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline bool IsClue(int y, int x) const { return field[CellId(y, x)] != CLUE_NONE; }
	inline int GetCellConstraint(int y, int x) const { return field[CellId(y, x)]; }
	inline void SetCellConstraint(int y, int x, int v) { field[CellId(y, x)] = v; }

	inline bool GetUseAssumption() const { return use_assumption; }
	inline void SetUseAssumption(bool assumption) { use_assumption = assumption; }
	inline SlitherlinkMethod GetMethod() const { return method; }
	inline void SetMethod(SlitherlinkMethod &met) { method = met; }

	static const int CLUE_NONE = -1;
	static const int CLUE_SOME = -2;

private:
	inline int CellId(int y, int x) const { return y * width + x; }

	int height, width;
	int *field;
	SlitherlinkMethod method;
	bool use_assumption;
};

}
