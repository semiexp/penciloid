#pragma once

#include "../common/GridLoop.hpp"

namespace Penciloid
{
class MasyuProblem;

class MasyuField : public GridLoop<MasyuField>
{
public:
	enum {
		CLUE_NONE = 0,
		CLUE_BLACK = 1,
		CLUE_WHITE = 2
	};

	MasyuField();
	~MasyuField();

	void Init(int height_t, int width_t);
	void Init(MasyuField &field);
	void Init(MasyuProblem &prob);

	inline int GetClue(int y, int x) { return clues[ClueId(y, x)]; }
	inline int GetClueSafe(int y, int x) { return (0 <= y && y <= GetHeight() && 0 <= x && x <= GetWidth()) ? GetClue(y, x) : CLUE_NONE; }
	int SetClue(int y, int x, int clue);
	inline int Assume() { return GridLoopAssume(*this); }

	void CheckVertexSpecific(int y, int x);

	void Debug();

private:
	void CheckTheorem(int y, int x);
	inline int ClueId(int y, int x) { return y * (GetWidth() + 1) + x; }

	int *clues;
};

}
