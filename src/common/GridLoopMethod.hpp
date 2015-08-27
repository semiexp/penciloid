#pragma once

namespace Penciloid
{
class GridLoopMethod
{
public:
	GridLoopMethod() : already_two_lines(true), avoid_cycle(true), hourglass_level1(true), hourglass_level2(true) {}
	bool already_two_lines, avoid_cycle;
	bool hourglass_level1;
	bool hourglass_level2;
};

}
