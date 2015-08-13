#pragma once

namespace Penciloid
{
class MasyuProblem
{
public:
	enum {
		CLUE_NONE = 0,
		CLUE_BLACK = 1,
		CLUE_WHITE = 2
	};

	MasyuProblem();
	~MasyuProblem();

	void Init(int height_t, int width_t);
	void Init(int height_t, int width_t, int *clue_t);
	void Init(int height_t, int width_t, char *clue_t[]);

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetClue(int y, int x) { return clue[CellId(y, x)]; }
	inline void SetClue(int y, int x, int h) { clue[CellId(y, x)] = h; }

private:
	inline int CellId(int y, int x) { return y * width + x; }

	int height, width;
	int *clue;
};

}
