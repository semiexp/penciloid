#include "Test.h"

namespace Penciloid
{
void PenciloidTest::RunTestAll()
{
	GridLoopTest1();
	GridLoopTest2a();
	GridLoopTest2b();
	GridLoopTest3();
	GridLoopTest4a();
	GridLoopTest4b();
	GridLoopTest5a();
	GridLoopTest5b();

	BlackCellForestTest1();

	SlitherlinkFieldTestByProblems();
	SlitherlinkReducedDatabaseTest();
	SlitherlinkReducedDatabaseCompletenessTest();

	MasyuTest1();
	MasyuTest2();
	MasyuTest3();
	MasyuTest4();

	KakuroTest1();

	ConsecutiveSolve("problem_test.txt");
}

}