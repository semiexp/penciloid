#pragma once

#include "../common/GridLoop.hpp"

namespace Penciloid
{
class MasyuProblem;

class MasyuField : public GridLoop<MasyuField>
{
public:
	enum {
		HINT_NONE = 0,
		HINT_BLACK = 1,
		HINT_WHITE = 2
	};

	MasyuField();
	~MasyuField();

	void Init(int height_t, int width_t);
	void Init(MasyuField &field);
	void Init(MasyuProblem &prob);

	inline int GetHint(int y, int x) { return hints[HintId(y, x)]; }
	inline int GetHintSafe(int y, int x) { return (0 <= y && y <= GetHeight() && 0 <= x && x <= GetWidth()) ? GetHint(y, x) : HINT_NONE; }
	int SetHint(int y, int x, int hint);
	inline int Assume() { return GridLoopAssume(*this); }

	void CheckVertexSpecific(int y, int x);

	void Debug();

private:
	void CheckTheorem(int y, int x);
	inline int HintId(int y, int x) { return y * (GetWidth() + 1) + x; }

	int *hints;
};

}
