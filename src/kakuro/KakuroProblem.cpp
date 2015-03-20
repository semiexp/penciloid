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

}
