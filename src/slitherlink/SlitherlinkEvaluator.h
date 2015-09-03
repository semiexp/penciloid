#include "SlitherlinkField.h"
#include <vector>

namespace Penciloid
{
class SlitherlinkEvaluator
{
public:
	// SlitherlinkEvaluator();
	// ~SlitherlinkEvaluator();

	void Init(SlitherlinkProblem &problem);

	double Evaluate();

	const double SCORE_IMPOSSIBLE = -1.0;
	const double SCORE_INCONSISTENT = -2.0;

private:
	enum {
		LOOP_UNDECIDED = SlitherlinkField::LOOP_UNDECIDED,
		LOOP_LINE = SlitherlinkField::LOOP_LINE,
		LOOP_BLANK = SlitherlinkField::LOOP_BLANK
	};

	struct move
	{
		move(double difficulty) : difficulty(difficulty) {}
		move(int y, int x, int style, double difficulty) : difficulty(difficulty) {
			ys.push_back(y); xs.push_back(x); styles.push_back(style);
		}
		void add(int y, int x, int style) {
			ys.push_back(y); xs.push_back(x); styles.push_back(style);
		}

		double difficulty;
		std::vector<int> ys, xs, styles;
	};

	void EnumerateValidMoves(std::vector<move> &moves);
	void EliminateDoneMoves(std::vector<move> &moves);

	void CheckAvoidCycleRule(std::vector<move> &moves);
	void CheckHourglassRule(int y, int x, std::vector<move> &moves);
	void CheckTwoLinesRule(std::vector<move> &moves);
	void CheckTheoremsAbout3(std::vector<move> &moves);
	bool CheckAdjacentLinesRule(int y, int x, std::vector<move> &moves);
	void CheckCornerCell(int y, int x, std::vector<move> &moves);
	void CheckLineToClue(int y, int x, std::vector<move> &moves);
	void CheckLineFromClue(int y, int x, std::vector<move> &moves);

	SlitherlinkField field;

	static const double DIFFICULTY_TWO_LINES;
	static const double DIFFICULTY_ADJACENT_LINES[4];
	static const double DIFFICULTY_ADJACENT_3, DIFFICULTY_DIAGONAL_3;
	static const double DIFFICULTY_CORNER_CLUE[4];
	static const double DIFFICULTY_LINE_TO_CLUE[4];
	static const double DIFFICULTY_LINE_FROM_CLUE[4];
};
}