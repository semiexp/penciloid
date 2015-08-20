#pragma once 

namespace Penciloid
{
class SlitherlinkDatabaseMethod;

class SlitherlinkDatabase
{
public:
	static void CreateDatabase();
	static void CreateReducedDatabase(SlitherlinkDatabaseMethod &method);
	static void ReleaseDatabase();
	static bool IsCreated() { return database != nullptr; }

	static int *database;
	static const int DATABASE_DY[12];
	static const int DATABASE_DX[12];

private:
	static int SolveLocal(int clue, int styles[12], SlitherlinkDatabaseMethod &method);
};

}
