namespace Penciloid
{
class SlitherlinkField;

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

	static void RunTestAll();
};
}