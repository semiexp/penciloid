#pragma once

namespace Penciloid
{
class SlitherlinkEvaluatorParameter
{
public:
	SlitherlinkEvaluatorParameter() {
		two_lines = 0.0;
		avoid_cycle = 1.2;
		hourglass_rule = 3.2;

		adjacent_lines[0] = 1.0;
		adjacent_lines[1] = 1.4;
		adjacent_lines[2] = 1.2;
		adjacent_lines[3] = 1.2;
		adjacent_3 = 2.7;
		diagonal_3 = 2.0;
		corner_clue[0] = 0.0;
		corner_clue[1] = 3.2;
		corner_clue[2] = 3.3;
		corner_clue[3] = 1.4;
		line_to_clue[0] = 0.0;
		line_to_clue[1] = 2.5;
		line_to_clue[2] = 1.2;
		line_to_clue[3] = 1.3;
		line_from_clue[0] = 0.0;
		line_from_clue[1] = 4.5;
		line_from_clue[2] = 0.0;
		line_from_clue[3] = 4.4;
		diagonal_chain = 3.1;
		inout_rule = 4.4;

		locality_base = 2.4;
		alternative_dimension = 3.0;
	}

	double two_lines;
	double avoid_cycle, hourglass_rule;

	double adjacent_lines[4];
	double adjacent_3, diagonal_3;
	double corner_clue[4];
	double line_to_clue[4];
	double line_from_clue[4];
	double diagonal_chain;
	double inout_rule;

	double locality_base;
	double alternative_dimension;
};
}
