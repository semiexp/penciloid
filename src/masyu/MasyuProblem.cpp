#include "MasyuProblem.h"

namespace Penciloid
{
MasyuProblem::MasyuProblem()
{
	clue = nullptr;
}
MasyuProblem::~MasyuProblem()
{
	if (clue) delete[] clue;
}
void MasyuProblem::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (clue) delete[] clue;
	clue = new int[height * width];

	for (int i = 0; i < height * width; ++i) clue[i] = CLUE_NONE;
}
void MasyuProblem::Init(int height_t, int width_t, int *clue_t)
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t * width_t; ++i) {
		if (1 <= clue[i] && clue[i] <= 2) clue[i] = clue_t[i];
	}
}
void MasyuProblem::Init(int height_t, int width_t, char *clue_t[])
{
	Init(height_t, width_t);

	for (int i = 0; i < height_t; ++i) {
		for (int j = 0; j < width_t; ++j) {
			if ('1' <= clue_t[i][j] && clue_t[i][j] <= '2') {
				SetClue(i, j, clue_t[i][j] - '0');
			}
		}
	}
}

}