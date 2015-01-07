#include "../slitherlink/SlitherlinkField.h"
#include "Test.h"

namespace Penciloid
{
void PenciloidTest::SlitherlinkTest1()
{
	SlitherlinkField field;

	field.Init(3, 3);
	
	field.SetHint(0, 0, 0);
	field.SetHint(0, 1, 2);
	field.SetHint(2, 0, 3);
	field.SetHint(2, 1, 1);

	field.Debug();
	fprintf(stderr, "%d\n", field.GetStatus());
}

}