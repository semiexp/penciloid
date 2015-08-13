#include "MasyuField.h"
#include "MasyuProblem.h"

namespace Penciloid
{
MasyuField::MasyuField()
{
	clues = nullptr;
}

MasyuField::~MasyuField()
{
	if (clues) delete[] clues;
}

void MasyuField::Init(int height_t, int width_t)
{
	GridLoop<MasyuField>::Init(height_t - 1, width_t - 1);

	if (clues) delete[] clues;
	clues = new int[(GetHeight() + 1) * (GetWidth() + 1)];

	for (int i = 0; i < (GetHeight() + 1) * (GetWidth() + 1); ++i) clues[i] = CLUE_NONE;
}

void MasyuField::Init(MasyuField &field)
{
	GridLoop<MasyuField>::Init(field);

	if (clues) delete[] clues;
	clues = new int[(GetHeight() + 1) * (GetWidth() + 1)];
	memcpy(clues, field.clues, sizeof(int) * (GetHeight() + 1) * (GetWidth() + 1));
}

void MasyuField::Init(MasyuProblem &prob)
{
	Init(prob.GetHeight(), prob.GetWidth());

	for (int i = 0; i <= GetHeight(); ++i) {
		for (int j = 0; j <= GetWidth(); ++j) {
			int clue = prob.GetClue(i, j);

			if (clue != CLUE_NONE) SetClue(i, j, clue);
		}
	}
}

int MasyuField::SetClue(int y, int x, int clue)
{
	int id = ClueId(y, x);

	if (clues[id] != CLUE_NONE) {
		if (clues[id] != clue) {
			return UpdateStatus(SolverStatus::INCONSISTENT);
		}
		return UpdateStatus(0);
	}

	clues[id] = clue;
	CheckTheorem(y * 2, x * 2);
	CheckVertex(y * 2, x * 2);

	return GetStatus();
}

void MasyuField::CheckTheorem(int y, int x)
{
	// TODO: implement appropriately

	int clue = GetClue(y / 2, x / 2); 

	if (clue == CLUE_WHITE) {
		for (int i = 0; i < 2; ++i) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];

			for (int j = 0; j <= 2; ++j) {
				if    (GetClueSafe(y / 2 + (j - 2) * dy, x / 2 + (j - 2) * dx) == CLUE_WHITE
					&& GetClueSafe(y / 2 + (j - 1) * dy, x / 2 + (j - 1) * dx) == CLUE_WHITE
					&& GetClueSafe(y / 2 + (j - 0) * dy, x / 2 + (j - 0) * dx) == CLUE_WHITE) {
					DetermineLine(y + 2 * (j - 2) * dy + dx, x + 2 * (j - 2) * dx + dy);
					DetermineLine(y + 2 * (j - 2) * dy - dx, x + 2 * (j - 2) * dx - dy);
					DetermineLine(y + 2 * (j - 1) * dy + dx, x + 2 * (j - 1) * dx + dy);
					DetermineLine(y + 2 * (j - 1) * dy - dx, x + 2 * (j - 1) * dx - dy);
					DetermineLine(y + 2 * (j - 0) * dy + dx, x + 2 * (j - 0) * dx + dy);
					DetermineLine(y + 2 * (j - 0) * dy - dx, x + 2 * (j - 0) * dx - dy);
				}
			}
		}
	}

	if (clue == CLUE_BLACK) {
		for (int i = 0; i < 4; ++i) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];

			if (GetClueSafe(y / 2 + dy, x / 2 + dx) == CLUE_BLACK) {
				DetermineBlank(y + dy, x + dx);
			}
		}
	}
}

void MasyuField::CheckVertexSpecific(int y, int x)
{
	if (clues == nullptr) return;

	int clue = clues[ClueId(y / 2, x / 2)];

	if (clue == CLUE_WHITE) {
		// TODO: faster checker
		int neighborhood_segment_style[16];

		for (int i = 0; i < 16; ++i) {
			neighborhood_segment_style[i] = GetSegmentStyleSafe(
				y + GridConstant::GRID_DY[i / 4] * 2 + GridConstant::GRID_DY[i % 4],
				x + GridConstant::GRID_DX[i / 4] * 2 + GridConstant::GRID_DX[i % 4]
				);
		}

		int segment_line_candidate = (1 << 16) - 1;
		int segment_blank_candidate = (1 << 16) - 1;

		do {
			if (neighborhood_segment_style[2] == LOOP_BLANK) break;
			if (neighborhood_segment_style[8] == LOOP_BLANK) break;
			if (neighborhood_segment_style[7] == LOOP_LINE) break;
			if (neighborhood_segment_style[13] == LOOP_LINE) break;

			int end1[] = { 0, 1, 3 };
			int end2[] = { 10, 9, 11 };

			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					if (i == 0 && j == 0) continue;

					if (neighborhood_segment_style[end1[i]] == LOOP_BLANK) continue;
					if (neighborhood_segment_style[end2[j]] == LOOP_BLANK) continue;
					if (neighborhood_segment_style[end1[(i + 1) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end2[(j + 1) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end1[(i + 2) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end2[(j + 2) % 3]] == LOOP_LINE) continue;

					int line_location = (1 << 2) | (1 << 8) | (1 << end1[i]) | (1 << end2[j]);
					int blank_location = ((15 << 0) | (15 << 8)) ^ line_location;

					segment_line_candidate &= line_location;
					segment_blank_candidate &= blank_location;
				}
			}
		} while (false);

		do {
			if (neighborhood_segment_style[7] == LOOP_BLANK) break;
			if (neighborhood_segment_style[13] == LOOP_BLANK) break;
			if (neighborhood_segment_style[2] == LOOP_LINE) break;
			if (neighborhood_segment_style[8] == LOOP_LINE) break;

			int end1[] = { 5, 4, 6 };
			int end2[] = { 15, 12, 14 };

			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					if (i == 0 && j == 0) continue;

					if (neighborhood_segment_style[end1[i]] == LOOP_BLANK) continue;
					if (neighborhood_segment_style[end2[j]] == LOOP_BLANK) continue;
					if (neighborhood_segment_style[end1[(i + 1) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end2[(j + 1) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end1[(i + 2) % 3]] == LOOP_LINE) continue;
					if (neighborhood_segment_style[end2[(j + 2) % 3]] == LOOP_LINE) continue;

					int line_location = (1 << 7) | (1 << 13) | (1 << end1[i]) | (1 << end2[j]);
					int blank_location = ((15 << 4) | (15 << 12)) ^ line_location;

					segment_line_candidate &= line_location;
					segment_blank_candidate &= blank_location;
				}
			}
		} while (false);

		for (int i = 0; i < 16; ++i) {
			if (segment_line_candidate & (1 << i)) {
				DetermineLine(
					y + GridConstant::GRID_DY[i / 4] * 2 + GridConstant::GRID_DY[i % 4],
					x + GridConstant::GRID_DX[i / 4] * 2 + GridConstant::GRID_DX[i % 4]
					);
			}

			if (segment_blank_candidate & (1 << i)) {
				DetermineBlank(
					y + GridConstant::GRID_DY[i / 4] * 2 + GridConstant::GRID_DY[i % 4],
					x + GridConstant::GRID_DX[i / 4] * 2 + GridConstant::GRID_DX[i % 4]
					);
			}
		}

	} else if (clue == CLUE_BLACK) {
		int line_direction_candidate = 15;

		for (int i = 0; i < 4; ++i) {
			int dy1 = GridConstant::GRID_DY[i], dx1 = GridConstant::GRID_DX[i];
			int dy2 = GridConstant::GRID_DY[(i + 1) & 3], dx2 = GridConstant::GRID_DX[(i + 1) & 3];

			if (GetSegmentStyleSafe(y + dy1, x + dx1) == LOOP_BLANK) continue;
			if (GetSegmentStyleSafe(y + dy1 * 3, x + dx1 * 3) == LOOP_BLANK) continue;
			if (GetSegmentStyleSafe(y + dy2, x + dx2) == LOOP_BLANK) continue;
			if (GetSegmentStyleSafe(y + dy2 * 3, x + dx2 * 3) == LOOP_BLANK) continue;
			if (GetSegmentStyleSafe(y + dy1 * 2 + dy2, x + dx1 * 2 + dx2) == LOOP_LINE) continue;
			if (GetSegmentStyleSafe(y + dy1 * 2 - dy2, x + dx1 * 2 - dx2) == LOOP_LINE) continue;
			if (GetSegmentStyleSafe(y + dy2 * 2 + dy1, x + dx2 * 2 + dx1) == LOOP_LINE) continue;
			if (GetSegmentStyleSafe(y + dy2 * 2 - dy1, x + dx2 * 2 - dx1) == LOOP_LINE) continue;

			line_direction_candidate &= ((1 << i) | (1 << ((i + 1) & 3)));
		}

		for (int i = 0; i < 4; ++i) if (line_direction_candidate & (1 << i)) {
			int dy = GridConstant::GRID_DY[i], dx = GridConstant::GRID_DX[i];
			DetermineLine(y + dy, x + dx);
			DetermineLine(y + dy * 3, x + dx * 3);
			DetermineBlank(y - dy, x - dx);
		}
	}
}

void MasyuField::Debug()
{
	for (int i = 0; i <= GetHeight() * 2; ++i) {
		for (int j = 0; j <= GetWidth() * 2; ++j) {
			if (i % 2 == 0 && j % 2 == 0) {
				int clue = clues[ClueId(i / 2, j / 2)];

				if (clue == CLUE_NONE) fprintf(stderr, "+");
				else if (clue == CLUE_WHITE) fprintf(stderr, "O");
				else if (clue == CLUE_BLACK) fprintf(stderr, "#");
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