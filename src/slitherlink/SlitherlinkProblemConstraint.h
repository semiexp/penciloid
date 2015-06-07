#pragma once

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
	void Init(int height_t, int width_t, int *hint_t, bool enable_hint_restriction = false);
	void Init(int height_t, int width_t, char *hint_t[], bool enable_hint_restriction = false);

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline bool IsHint(int y, int x) const { return field[CellId(y, x)] != HINT_NONE; }
	inline int GetCellConstraint(int y, int x) const { return field[CellId(y, x)]; }
	inline void SetCellConstraint(int y, int x, int v) { field[CellId(y, x)] = v; }

	static const int HINT_NONE = -1;
	static const int HINT_SOME = -2;

private:
	inline int CellId(int y, int x) const { return y * width + x; }

	int height, width;
	int *field;
};

}
