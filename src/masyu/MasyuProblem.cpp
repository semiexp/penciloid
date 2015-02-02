#include "MasyuProblem.h"

namespace Penciloid
{
MasyuProblem::MasyuProblem()
{
	hint = nullptr;
}
MasyuProblem::~MasyuProblem()
{
	if (hint) delete[] hint;
}
void MasyuProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	hint = new int[height * width];

	for (int i = 0; i < height * width; ++i) hint[i] = HINT_NONE;
}
void MasyuProblem::Init(int height_t, int width_t, int *hint_t)
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t * width_t; ++i) {
		if (1 <= hint[i] && hint[i] <= 2) hint[i] = hint_t[i];
	}
}
void MasyuProblem::Init(int height_t, int width_t, char *hint_t[])
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t; ++i) {
		for (int j = 0; j < width_t; ++j) {
			if ('1' <= hint_t[i][j] && hint_t[i][j] <= '2') {
				SetHint(i, j, hint_t[i][j] - '0');
			}
		}
	}
}

}