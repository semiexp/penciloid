#pragma once

namespace Penciloid
{
class SlitherlinkEvaluatorParameter
{
public:
	SlitherlinkEvaluatorParameter() {
		two_lines = 0.0;
		adjacent_lines[0] = 0.3;
		adjacent_lines[1] = 1.5;
		adjacent_lines[2] = 1.5;
		adjacent_lines[3] = 1.5;
		adjacent_3 = 3.0;
		diagonal_3 = 3.0;
		corner_clue[0] = 0.0;
		corner_clue[1] = 2.0;
		corner_clue[2] = 2.0;
		corner_clue[3] = 2.0;
		line_to_clue[0] = 0.0;
		line_to_clue[1] = 2.0;
		line_to_clue[2] = 2.0;
		line_to_clue[3] = 2.0;
		line_from_clue[0] = 0.0;
		line_from_clue[1] = 4.5;
		line_from_clue[2] = 0.0;
		line_from_clue[3] = 4.5;
		diagonal_chain = 3.0;
	}

	double two_lines;
	double adjacent_lines[4];
	double adjacent_3, diagonal_3;
	double corner_clue[4];
	double line_to_clue[4];
	double line_from_clue[4];
	double diagonal_chain;
};
}
