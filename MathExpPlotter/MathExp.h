#pragma once

#include <windows.h>
#include <string>
#include <cmath>
#include <vector>
#include <stack>
#include <set>
#include <regex>

using std::sin;
using std::cos;
using std::tan;
using std::string;
using std::stack;
using std::vector;
using std::set;

static const double PI = 3.14159265358979;

namespace MathExpPlotter
{
	class MathExp
	{
	public:
		MathExp(string _math_exp_str);
		MathExp(LPCSTR _math_exp_lpcstr);
		~MathExp();

		void initialize();
	private:
		string math_exp_str;
		std::set<char> op_set;
	};
}