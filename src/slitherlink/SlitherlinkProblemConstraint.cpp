#include "SlitherlinkProblemConstraint.h"

namespace Penciloid
{
void SlitherlinkProblemConstraint::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height * width; ++i) field[i] = CLUE_NONE;
}
void SlitherlinkProblemConstraint::Init(int height_t, int width_t, int* clue_t, bool enable_clue_restriction)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height * width; ++i) {
		if (!enable_clue_restriction) field[i] = (clue_t[i] == 0 ? CLUE_NONE : CLUE_SOME);
		else field[i] = clue_t[i];
	}
}
void SlitherlinkProblemConstraint::Init(int height_t, int width_t, char* clue_t[], bool enable_clue_restriction)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (!enable_clue_restriction) field[CellId(i, j)] = (clue_t[i][j] == '.' ? CLUE_NONE : CLUE_SOME);
			else {
				if (clue_t[i][j] >= '0' && clue_t[i][j] <= '3') field[CellId(i, j)] = clue_t[i][j] - '0';
				else if (clue_t[i][j] == '.') field[CellId(i, j)] = CLUE_NONE;
				else field[CellId(i, j)] = CLUE_SOME;
			}
		}
	}
}

}
