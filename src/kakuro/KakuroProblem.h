#pragma once

namespace Penciloid
{
class KakuroProblem
{
public:
	KakuroProblem() : height(0), width(0), clue(nullptr) {}
	~KakuroProblem() { if (clue) delete[] clue; }

	void Init(int height_t, int width_t);
	void Init(int height_t, int width_t, int *clue_t);

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline int GetClueVertical(int y, int x) const { return clue[CellId(y, x)]; }
	inline int GetClueHorizontal(int y, int x) const { return clue[CellId(y, x) | 1]; }
	inline bool IsNumberCell(int y, int x) const { return clue[CellId(y, x)] == NUMBER_CELL; }
	inline bool IsClueAvailableVertical(int y, int x) const { return GetClueVertical(y, x) >= 0; }
	inline bool IsClueAvailableHorizontal(int y, int x) const { return GetClueHorizontal(y, x) >= 0; }

	inline void SetNumberCell(int y, int x) { clue[CellId(y, x)] = clue[CellId(y, x) | 1] = NUMBER_CELL; }
	inline void SetClue(int y, int x, int vertical, int horizontal) {
		clue[CellId(y, x)] = vertical;
		clue[CellId(y, x) | 1] = horizontal;
	}

	static const int CLUE_NONE = -1;
	static const int NUMBER_CELL = -2;

private:
	inline int CellId(int y, int x) const { return (y * width + x) * 2; }

	int height, width;
	int *clue;
};

}
