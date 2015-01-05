#include "../common/GridLoop.hpp"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::GridLoopTest1()
{
	GridLoop<LoopNullVertex, LoopNullCell> grid;
	grid.Init(5, 5);

	grid.DetermineLine(0, 1);
	grid.DetermineLine(1, 4);
	grid.DetermineLine(1, 2);
	grid.DetermineLine(4, 1);
	grid.DetermineBlank(4, 3);

	grid.Debug();
	grid.Debug2();
}

}