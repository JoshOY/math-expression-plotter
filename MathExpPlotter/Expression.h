#pragma once

#include <vector>
#include <string>
#include <iterator>
#include <windows.h>

using std::string;
using std::vector;

struct Expression
{
	Expression();
	Expression(string _init_exp_str,
		int init_exp_id,
		COLORREF _exp_color,
		double _rangeStart,
		double _rangeEnd);

	string exp_str;
	int exp_id;
	COLORREF exp_color = 0x33bbff;
	double rangeStart;
	double rangeEnd;
	bool enabled;

	bool operator==(const Expression & rVal) const;
	bool operator<(const Expression & rVal) const;
};

std::string MBFromW(LPCWSTR pwsz, UINT cp);
