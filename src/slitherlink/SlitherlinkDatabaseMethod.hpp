#pragma once

namespace Penciloid
{
class SlitherlinkDatabaseMethod
{
public:
	SlitherlinkDatabaseMethod() :
		vertex_rule(true), adjacent_lines(true), corner_1(true), corner_2(true), corner_3(true),
		line_to_1(true), line_to_2(true), line_to_3(true), line_from_1(true), line_from_3(true) {}

	bool vertex_rule;
	bool adjacent_lines;
	bool corner_1, corner_2, corner_3;
	bool line_to_1, line_to_2, line_to_3;
	bool line_from_1, line_from_3;
};
}
