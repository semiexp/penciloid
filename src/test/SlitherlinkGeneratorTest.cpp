#include <ctime>
#include "Test.h"
#include "../slitherlink/SlitherlinkDatabase.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkGenerator.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkGeneratorTest(int height, int width)
{
	srand((int)time(NULL));
	SlitherlinkDatabase::CreateDatabase();

	SlitherlinkProblem prob;
	SlitherlinkGenerator::GenerateNaive(height, width, prob);

	SlitherlinkField field;
	field.Init(prob);
	field.Assume();

	field.Debug();
}

}
