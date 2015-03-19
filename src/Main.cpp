#include "test/Test.h"
#include <cstdio>

int main()
{
	Penciloid::PenciloidTest::RunTestAll();

	char c;
	printf("Press any key to continue...");
	scanf_s("%c", &c);
	
	return 0;
}