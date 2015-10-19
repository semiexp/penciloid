#include <iostream>
#include <vector>

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
	static void SlitherlinkClueTest(int height, int width, std::vector<const char*> board, int expected_status, bool use_diagonal_chain, int test_id);
	static void SlitherlinkFieldTestByProblems();
	static void SlitherlinkReducedDatabaseTest();
	static void SlitherlinkReducedDatabaseCompletenessTest();

	static void SlitherlinkProblemTest(SlitherlinkProblem &problem);

	static void MasyuTest1();
	static void MasyuTest2();
	static void MasyuTest3();
	static void MasyuTest4();

	static void KakuroTest1();

	static bool InputSlitherlink(std::istream &ifs, SlitherlinkProblem &problem);
	static bool InputMasyu(std::istream &ifs, MasyuProblem &problem);
	static bool InputKakuro(std::istream &ifs, KakuroProblem &problem);

	static void SlitherlinkGeneratorTest(int height, int width);
	static void SlitherlinkGeneratorOfShapeTest();

	static void ConsecutiveSolve(const char* filename);
	static void ConsecutivePerformanceTest(const char* filename);

	static void RunTestAll();
};
}