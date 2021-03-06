#pragma once

#include <string>

namespace Penciloid
{
class SlitherlinkProblem
{
public:
	SlitherlinkProblem();
	SlitherlinkProblem(const SlitherlinkProblem &src);
	~SlitherlinkProblem();

	void Init(int height_t, int width_t);
	void Init(int height_t, int width_t, int *clue_t);
	void Init(int height_t, int width_t, char *clue_t[]);

	inline int GetHeight() { return height; }
	inline int GetWidth() { return width; }
	inline int GetClue(int y, int x) { return clue[CellId(y, x)]; }
	inline void SetClue(int y, int x, int h) { clue[CellId(y, x)] = h; }

	std::string GetPlayerFormatString();

private:
	inline int CellId(int y, int x) { return y * width + x; }

	int height, width;
	int *clue;
};

}
