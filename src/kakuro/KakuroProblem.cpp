#include "KakuroProblem.h"

namespace Penciloid
{
void KakuroProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (clue) delete[] clue;
	clue = new int[height * width * 2];

	for (int i = 0; i < height * width * 2; ++i) {
		clue[i] = CLUE_NONE;
	}
}
void KakuroProblem::Init(int height_t, int width_t, int *clue_t)
{
	Init(height_t, width_t);
	for (int i = 0; i < height * width * 2; ++i) clue[i] = clue_t[i];
}

}
