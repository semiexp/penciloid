#include <cassert>
#include <cstdio>

#include "../common/GridLoop.hpp"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::GridLoopTest1()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 0);

	assert(grid.GetSegmentStyle(2, 1) == grid.LOOP_BLANK);
	assert(grid.GetSegmentStyle(3, 0) == grid.LOOP_LINE);
}
void PenciloidTest::GridLoopTest2a()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineBlank(3, 0);
	grid.DetermineLine(0, 1);

	assert(grid.GetStatus() & SolverStatus::SUCCESS);
	grid.DetermineLine(4, 1);
	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest2b()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineBlank(0, 3);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(2, 1);

	assert(grid.GetStatus() & SolverStatus::SUCCESS);
	grid.DetermineLine(4, 1);
	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest3()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineLine(0, 1);
	grid.DetermineLine(2, 1);
	grid.DetermineLine(5, 0);
	grid.DetermineLine(1, 2);

	assert(grid.GetStatus() & SolverStatus::INCONSISTENT);
}
void PenciloidTest::GridLoopTest4a()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineLine(5, 6);
	grid.DetermineLine(1, 4);
	grid.DetermineLine(2, 3);
	grid.DetermineLine(0, 1);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(3, 0) == GridLoop<LoopNullAuxiliarySolver>::LOOP_LINE);
}
void PenciloidTest::GridLoopTest4b()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineLine(1, 4);
	grid.DetermineLine(2, 3);
	grid.DetermineLine(0, 1);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(3, 0) == GridLoop<LoopNullAuxiliarySolver>::LOOP_UNDECIDED);
}
void PenciloidTest::GridLoopTest5a()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineLine(3, 2);
	grid.DetermineLine(3, 4);
	grid.DetermineLine(4, 1);
	grid.DetermineLine(4, 5);
	grid.DetermineLine(6, 3);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(0, 3);
	grid.DetermineLine(0, 5);

	assert(grid.GetStatus() == SolverStatus::SUCCESS);
	assert(grid.GetSegmentStyle(2, 3) == GridLoop<LoopNullAuxiliarySolver>::LOOP_BLANK);
}
void PenciloidTest::GridLoopTest5b()
{
	GridLoop<LoopNullAuxiliarySolver> grid;
	grid.Init(3, 3);

	grid.DetermineLine(3, 2);
	grid.DetermineLine(3, 4);
	grid.DetermineLine(4, 1);
	grid.DetermineLine(4, 5);
	grid.DetermineLine(0, 1);
	grid.DetermineLine(0, 3);
	grid.DetermineLine(0, 5);

	assert(!(grid.GetStatus() & SolverStatus::INCONSISTENT));
	assert(grid.GetSegmentStyle(2, 3) == GridLoop<LoopNullAuxiliarySolver>::LOOP_UNDECIDED);
}

}