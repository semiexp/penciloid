namespace Penciloid
{
class SlitherlinkField;
class SlitherlinkProblem;

class PenciloidTest
{
public:
	static void GridLoopTest1();
	static void GridLoopTest2a();
	static void GridLoopTest2b();
	static void GridLoopTest3();

	static bool SlitherlinkCheckGrid(SlitherlinkField &field, const int *expected);
	static void SlitherlinkTest1();
	static void SlitherlinkTest2();
	static void SlitherlinkTest3();

	static void SlitherlinkProblemTest(SlitherlinkProblem &problem);
	static void SlitherlinkLoadProblem(SlitherlinkProblem &storage, int problem_id);

	static void RunTestAll();

	static const int NUMBER_OF_SLITHERLINK_PROBLEM;
};
}