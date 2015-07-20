#include "SlitherlinkProblemConstraint.h"

namespace Penciloid
{
void SlitherlinkProblemConstraint::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height * width; ++i) field[i] = HINT_NONE;
}
void SlitherlinkProblemConstraint::Init(int height_t, int width_t, int* hint_t, bool enable_hint_restriction)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height * width; ++i) {
		if (!enable_hint_restriction) field[i] = (hint_t[i] == 0 ? HINT_NONE : HINT_SOME);
		else field[i] = hint_t[i];
	}
}
void SlitherlinkProblemConstraint::Init(int height_t, int width_t, char* hint_t[], bool enable_hint_restriction)
{
	height = height_t;
	width = width_t;

	if (field) delete[] field;
	field = new int[height * width];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (!enable_hint_restriction) field[CellId(i, j)] = (hint_t[i][j] == '.' ? HINT_NONE : HINT_SOME);
			else {
				if (hint_t[i][j] >= '0' && hint_t[i][j] <= '3') field[CellId(i, j)] = hint_t[i][j] - '0';
				else if (hint_t[i][j] == '.') field[CellId(i, j)] = HINT_NONE;
				else field[CellId(i, j)] = HINT_SOME;
			}
		}
	}
}

}
