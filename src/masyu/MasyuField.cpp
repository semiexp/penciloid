#include "MasyuField.h"

namespace Penciloid
{
MasyuField::MasyuField()
{
	height = 0;
	width = 0;

	hints = nullptr;
}

MasyuField::~MasyuField()
{
	if (hints) delete[] hints;
}

void MasyuField::Init(int height_t, int width_t)
{
	height = height_t;
	width = width_t;

	if (hints) delete[] hints;
	hints = new int[height * width];

	for (int i = 0; i < height * width; ++i) hints[i] = HINT_NONE;

	auxiliary.Init(this);
	grid.SetAuxiliarySolver(&auxiliary);
	grid.Init(height - 1, width - 1);
}

int MasyuField::SetHint(int y, int x, int hint)
{
	int id = VertexId(y, x);

	if (hints[id] != HINT_NONE) {
		if (hints[id] != hint) {
			return grid.UpdateStatus(SolverStatus::INCONSISTENT);
		}
		return grid.UpdateStatus(0);
	}

	hints[id] = hint;
	CheckTheorem(y * 2, x * 2);
	CheckVertex(y * 2, x * 2);

	return grid.GetStatus();
}

void MasyuField::CheckTheorem(int y, int x)
{
	// TODO: implement appropriately
}

void MasyuField::CheckVertex(int y, int x)
{
	int hint = hints[VertexId(y/ 2, x / 2)];

	if (hint == HINT_WHITE) {
		// TODO: implement rule for hint of white circle

	} else if (hint == HINT_BLACK) {
		int line_direction_candidate = 15;

		for (int i = 0; i < 4; ++i) {
			int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
			int dy2 = GridConstant::GRID_DY[(i + 1) & 3], dx2 = GridConstant::GRID_DX[(i + 1) & 3];

			if (grid.GetSegmentStyleSafe(y + dy1, x + dx1) == LOOP_BLANK) continue;
			if (grid.GetSegmentStyleSafe(y + dy1 * 3, x + dx1 * 3) == LOOP_BLANK) continue;
			if (grid.GetSegmentStyleSafe(y + dy2, x + dx2) == LOOP_BLANK) continue;
			if (grid.GetSegmentStyleSafe(y + dy2 * 3, x + dx2 * 3) == LOOP_BLANK) continue;
			if (grid.GetSegmentStyleSafe(y + dy1 * 2 + dy2, x + dx1 * 2 + dx2) == LOOP_LINE) continue;
			if (grid.GetSegmentStyleSafe(y + dy1 * 2 - dy2, x + dx1 * 2 - dx2) == LOOP_LINE) continue;
			if (grid.GetSegmentStyleSafe(y + dy2 * 2 + dy1, x + dx2 * 2 + dx1) == LOOP_LINE) continue;
			if (grid.GetSegmentStyleSafe(y + dy2 * 2 - dy1, x + dx2 * 2 - dx1) == LOOP_LINE) continue;

			line_direction_candidate &= ((1 << i) | (1 << ((i + 1) & 3)));
		}

		for (int i = 0; i < 4; ++i) if (line_direction_candidate & (1 << i)) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];
			grid.DetermineLine(y + dy, x + dx);
			grid.DetermineLine(y + dy * 3, x + dx * 3);
		}
	}
}

void MasyuField::Debug()
{
	for (int i = 0; i <= (height - 1) * 2; ++i) {
		for (int j = 0; j <= (width - 1) * 2; ++j) {
			if (i % 2 == 0 && j % 2 == 0) {
				int hint = hints[VertexId(i / 2, j / 2)];

				if (hint == HINT_NONE) fprintf(stderr, "+");
				else if (hint == HINT_WHITE) fprintf(stderr, "O");
				else if (hint == HINT_BLACK) fprintf(stderr, "#");
			} else if (i % 2 == 0 && j % 2 == 1) {
				int style = GetSegmentStyle(i, j);
				if (style == LOOP_UNDECIDED) fprintf(stderr, "   ");
				if (style == LOOP_LINE) fprintf(stderr, "---");
				if (style == LOOP_BLANK) fprintf(stderr, " X ");
			} else if (i % 2 == 1 && j % 2 == 0) {
				int style = GetSegmentStyle(i, j);
				if (style == LOOP_UNDECIDED) fprintf(stderr, " ");
				if (style == LOOP_LINE) fprintf(stderr, "|");
				if (style == LOOP_BLANK) fprintf(stderr, "X");
			} else if (i % 2 == 1 && j % 2 == 1) {
				fprintf(stderr, "   ");
			}
		}
		fprintf(stderr, "\n");
	}
}

}