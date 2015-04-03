#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

#include "Test.h"
#include "../masyu/MasyuField.h"
#include "../masyu/MasyuProblem.h"
#include "../slitherlink/SlitherlinkField.h"
#include "../slitherlink/SlitherlinkProblem.h"
#include "../slitherlink/SlitherlinkDatabase.h"
#include "../kakuro/KakuroField.h"
#include "../kakuro/KakuroProblem.h"

namespace Penciloid
{
void PenciloidTest::ConsecutiveSolve(const char* filename)
{
	SlitherlinkDatabase::CreateDatabase();

	std::ifstream ifs(filename);

	while (!ifs.eof()) {
		std::string puzzle_type, probid;
		ifs >> puzzle_type >> probid;

		if (puzzle_type == std::string("slitherlink")) {
			SlitherlinkProblem problem;
			InputSlitherlink(ifs, problem);

			SlitherlinkField field;
			field.Init(problem);

			field.Assume();

			std::cout << "Slitherlink [" << probid << "]: ";
			std::cerr << "Slitherlink [" << probid << "]: ";

			if (field.GetStatus() == SolverStatus::SUCCESS) {
				std::cout << "OK" << std::endl;
				std::cerr << "OK" << std::endl;
			} else {
				std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
				std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
				field.Debug();
			}
		} else if (puzzle_type == std::string("masyu")) {
			MasyuProblem problem;
			InputMasyu(ifs, problem);

			MasyuField field;
			field.Init(problem);

			field.Assume();

			std::cout << "Masyu [" << probid << "]: ";
			std::cerr << "Masyu [" << probid << "]: ";

			if (field.GetStatus() == SolverStatus::SUCCESS) {
				std::cout << "OK" << std::endl;
				std::cerr << "OK" << std::endl;
			} else {
				std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
				std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
				field.Debug();
			}
		} else if (puzzle_type == std::string("kakuro")) {
			KakuroProblem problem;
			InputKakuro(ifs, problem);

			KakuroField field;
			field.Init(problem);
			field.CheckAll();

			std::cout << "Kakuro [" << probid << "]: ";
			std::cerr << "Kakuro [" << probid << "]: ";

			if (field.GetStatus() == SolverStatus::SUCCESS) {
				std::cout << "OK" << std::endl;
				std::cerr << "OK" << std::endl;
			} else {
				std::cout << "Fail (" << field.GetStatus() << ")" << std::endl;
				std::cerr << "Fail (" << field.GetStatus() << ")" << std::endl;
				field.Debug();
			}
		} else {
			break;
		}
	}
}
void PenciloidTest::ConsecutivePerformanceTest(const char* filename)
{
	SlitherlinkDatabase::CreateDatabase();

	std::ifstream ifs(filename);
	time_t start, end;

	start = clock();
	while (!ifs.eof()) {
		std::string puzzle_type, probid;
		int repeat, status = 0;
		ifs >> puzzle_type >> probid >> repeat;

		if (puzzle_type == std::string("slitherlink")) {
			SlitherlinkProblem problem;
			InputSlitherlink(ifs, problem);

			for (int i = 0; i < repeat; ++i) {
				SlitherlinkField field;
				field.Init(problem);

				field.Assume();

				status += field.GetStatus();
			}
		} else if (puzzle_type == std::string("masyu")) {
			MasyuProblem problem;
			InputMasyu(ifs, problem);

			for (int i = 0; i < repeat; ++i) {
				MasyuField field;
				field.Init(problem);

				field.Assume();

				status += field.GetStatus();
			}
		} else if (puzzle_type == std::string("kakuro")) {
			KakuroProblem problem;
			InputKakuro(ifs, problem);

			for (int i = 0; i < repeat; ++i) {
				KakuroField field;
				field.Init(problem);
				field.CheckAll();

				status += field.GetStatus();
			}
		} else {
			break;
		}

		status /= repeat;
		if (status != SolverStatus::SUCCESS) {
			std::cerr << "Problem [" << probid << "]: Fail (" << status << ")" << std::endl;
		}
	}
	end = clock();

	std::cerr << "Done. (Cost: " << (double)(end - start) / CLOCKS_PER_SEC << ")" << std::endl;
}

}
