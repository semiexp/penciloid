#pragma once

namespace Penciloid
{
class KakuroProblem
{
public:
	KakuroProblem() : height(0), width(0), hint(nullptr) {}
	~KakuroProblem() { if (hint) delete[] hint; }

	void Init(int height_t, int width_t);

	inline int GetHeight() const { return height; }
	inline int GetWidth() const { return width; }
	inline int GetHintVertical(int y, int x) const { return hint[CellId(y, x)]; }
	inline int GetHintHorizontal(int y, int x) const { return hint[CellId(y, x) | 1]; }
	inline bool IsNumberCell(int y, int x) const { return hint[CellId(y, x)] == NUMBER_CELL; }
	inline bool IsHintAvailableVertical(int y, int x) const { return GetHintVertical(y, x) >= 0; }
	inline bool IsHintAvailableHorizontal(int y, int x) const { return GetHintHorizontal(y, x) >= 0; }

	inline void SetNumberCell(int y, int x) { hint[CellId(y, x)] = hint[CellId(y, x) | 1] = NUMBER_CELL; }
	inline void SetHint(int y, int x, int vertical, int horizontal) {
		hint[CellId(y, x)] = vertical;
		hint[CellId(y, x) | 1] = horizontal;
	}

	static const int HINT_NONE = -1;
	static const int NUMBER_CELL = -2;

private:
	inline int CellId(int y, int x) const { return (y * width + x) * 2; }

	int height, width;
	int *hint;
};

}
