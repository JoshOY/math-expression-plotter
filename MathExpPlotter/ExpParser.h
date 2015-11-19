#pragma once

#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

namespace MathExpPlotter {

	class ExpParser {
	public:
		ExpParser();
		ExpParser(const string s_init);
		~ExpParser();

		void     calculateValueFromRange(double x_start, double x_end, int stepNum);
		double * get_y_vec();
		double   get_y_at(int idx);
		double   get_x_at(int idx);
	private:
		string mathexp_str;
		double * x_vec;
		double * y_vec;
		int step_total;
	};


}