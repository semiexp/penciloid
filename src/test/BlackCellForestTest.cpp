#include <cassert>
#include <cstdio>

#include "../common/BlackCellForest.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::BlackCellForestTest1()
{
	BlackCellForest forest;
	forest.Init(5, 6);

	assert(!forest.IsArticulationPoint(1, 0));
	forest.Blacken(0, 1);
	assert(forest.IsArticulationPoint(1, 0));
	assert(!forest.IsArticulationPoint(0, 3));
	forest.Blacken(1, 2);
	assert(forest.IsArticulationPoint(0, 3));
	assert(!forest.IsArticulationPoint(3, 0));
	forest.Blacken(2, 1);
	assert(forest.IsArticulationPoint(3, 0));
	assert(!forest.IsArticulationPoint(2, 3));
	assert(!forest.IsArticulationPoint(4, 1));
	assert(!forest.IsArticulationPoint(4, 3));
	forest.Blacken(3, 2);
	assert(forest.IsArticulationPoint(2, 3));
	assert(forest.IsArticulationPoint(4, 1));
	assert(forest.IsArticulationPoint(4, 3));
	assert(!forest.IsArticulationPoint(0, 4));
	forest.Blacken(1, 5);
	assert(forest.IsArticulationPoint(0, 4));
	assert(!forest.IsArticulationPoint(3, 5));
	forest.Blacken(4, 4);
	assert(forest.IsArticulationPoint(3, 5));
}

}
