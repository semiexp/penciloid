#pragma once

namespace Penciloid
{
class SlitherlinkEvaluatorParameter
{
public:
	SlitherlinkEvaluatorParameter() {
		two_lines = 0.0;
		adjacent_lines[0] = 0.6;
		adjacent_lines[1] = 1.4;
		adjacent_lines[2] = 1.4;
		adjacent_lines[3] = 1.3;
		adjacent_3 = 3.0;
		diagonal_3 = 2.5;
		corner_clue[0] = 0.0;
		corner_clue[1] = 2.2;
		corner_clue[2] = 2.6;
		corner_clue[3] = 2.1;
		line_to_clue[0] = 0.0;
		line_to_clue[1] = 2.4;
		line_to_clue[2] = 2.0;
		line_to_clue[3] = 1.6;
		line_from_clue[0] = 0.0;
		line_from_clue[1] = 4.0;
		line_from_clue[2] = 0.0;
		line_from_clue[3] = 3.8;
		diagonal_chain = 2.9;

		locality_base = 2.8;
		alternative_dimension = 2.4;
	}

	double two_lines;
	double adjacent_lines[4];
	double adjacent_3, diagonal_3;
	double corner_clue[4];
	double line_to_clue[4];
	double line_from_clue[4];
	double diagonal_chain;

	double locality_base;
	double alternative_dimension;
};
}
