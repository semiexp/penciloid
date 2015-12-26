#pragma once

namespace Penciloid
{
class SlitherlinkEvaluatorParameter
{
public:
	SlitherlinkEvaluatorParameter() {
		two_lines = 0.0;
		avoid_cycle = 1.2;
		hourglass_rule = 3.8;

		adjacent_lines[0] = 0.7;
		adjacent_lines[1] = 1.5;
		adjacent_lines[2] = 1.2;
		adjacent_lines[3] = 1.2;
		adjacent_3 = 2.9;
		diagonal_3 = 2.3;
		corner_clue[0] = 0.0;
		corner_clue[1] = 2.7;
		corner_clue[2] = 2.6;
		corner_clue[3] = 1.6;
		line_to_clue[0] = 0.0;
		line_to_clue[1] = 2.6;
		line_to_clue[2] = 1.2;
		line_to_clue[3] = 1.5;
		line_from_clue[0] = 0.0;
		line_from_clue[1] = 4.3;
		line_from_clue[2] = 0.0;
		line_from_clue[3] = 4.0;
		diagonal_chain = 2.7;
		inout_rule = 3.6;

		locality_base = 2.8;
		alternative_dimension = 2.4;
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
