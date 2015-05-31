#pragma once

namespace Penciloid
{
class GridLoopMethod
{
public:
	GridLoopMethod() : hourglass_level1(true), hourglass_level2(true) {}

	bool hourglass_level1;
	bool hourglass_level2;
};

}
