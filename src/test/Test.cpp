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

	SlitherlinkTest1();
	SlitherlinkTest2();
	SlitherlinkTest3();

	MasyuTest1();
	MasyuTest2();
	MasyuTest3();
	
	KakuroTest1();

	ConsecutiveSolve("problem_test.txt");
}

}