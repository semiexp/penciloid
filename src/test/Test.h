namespace Penciloid
{
class SlitherlinkField;
class SlitherlinkProblem;
class MasyuField;
class MasyuProblem;
class KakuroProblem;

class PenciloidTest
{
public:
	static void GridLoopTest1();
	static void GridLoopTest2a();
	static void GridLoopTest2b();
	static void GridLoopTest3();
	static void GridLoopTest4a();
	static void GridLoopTest4b();
	static void GridLoopTest5a();
	static void GridLoopTest5b();

	static void BlackCellForestTest1();

	static bool SlitherlinkCheckGrid(SlitherlinkField &field, const int *expected);
	static void SlitherlinkTest1();
	static void SlitherlinkTest2();
	static void SlitherlinkTest3();
	static void SlitherlinkPerformanceTest();

	static void SlitherlinkProblemTest(SlitherlinkProblem &problem);
	static void SlitherlinkLoadProblem(SlitherlinkProblem &storage, int problem_id);

	static void MasyuTest1();
	static void MasyuTest2();
	static void MasyuTest3();
	static void MasyuTest4();
	static void MasyuPerformanceTest();

	static void MasyuLoadProblem(MasyuProblem &storage, int problem_id);

	static void KakuroTest1();
	static void KakuroConsecutiveSolve(const char* filename);

	static void RunTestAll();

	static const int NUMBER_OF_SLITHERLINK_PROBLEM;
	static const int NUMBER_OF_MASYU_PROBLEM;
	static const int NUMBER_OF_KAKURO_PROBLEM;
};
}