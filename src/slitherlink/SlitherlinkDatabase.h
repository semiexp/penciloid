#pragma once 

namespace Penciloid
{

class SlitherlinkDatabase
{
public:
	static void CreateDatabase();
	static void ReleaseDatabase();
	static bool IsCreated() { return database != nullptr; }

	static int *database;
	static const int DATABASE_DY[12];
	static const int DATABASE_DX[12];
};

}
