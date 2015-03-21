#include "KakuroProblem.h"

namespace Penciloid
{
void KakuroProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (hint) delete[] hint;
	hint = new int[height * width * 2];

	for (int i = 0; i < height * width * 2; ++i) {
		hint[i] = HINT_NONE;
	}
}
void KakuroProblem::Init(int height_t, int width_t, int *hint_t)
{
	Init(height_t, width_t);
	for (int i = 0; i < height * width * 2; ++i) hint[i] = hint_t[i];
}

}
