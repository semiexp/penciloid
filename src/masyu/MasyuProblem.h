#pragma once

namespace Penciloid
{
class MasyuProblem
{
public:
	enum {
		HINT_NONE = 0,
		HINT_BLACK = 1,
		HINT_WHITE = 2
	};

	MasyuProblem();
	~MasyuProblem();

	void Init(int height_t, int width_t);
	void Init(int height_t, int width_t, int *hint_t);
	void Init(int height_t, int width_t, char *hint_t[]);

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetHint(int y, int x) { return hint[CellId(y, x)]; }
	inline void SetHint(int y, int x, int h) { hint[CellId(y, x)] = h; }

private:
	inline int CellId(int y, int x) { return y * width + x; }

	int height, width;
	int *hint;
};

}
