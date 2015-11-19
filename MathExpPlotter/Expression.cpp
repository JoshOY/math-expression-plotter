#include "Expression.h"

Expression::Expression()
{
	this->exp_str = "";
	this->exp_id = -1;
	this->exp_color = 0x000000;
	this->rangeStart = 0;
	this->rangeEnd = 0;
	this->enabled = true;
}

Expression::Expression(
	string _init_exp_str,
	int _init_exp_id,
	COLORREF _exp_color,
	double _rangeStart,
	double _rangeEnd
	)
{
	this->exp_str = _init_exp_str;
	this->exp_id = _init_exp_id;
	this->exp_color = _exp_color;
	this->rangeStart = _rangeStart;
	this->rangeEnd = _rangeEnd;
	this->enabled = true;
}

std::string MBFromW(LPCWSTR pwsz, UINT cp)
{
	int cch = WideCharToMultiByte(cp, 0, pwsz, -1, 0, 0, NULL, NULL);

	char* psz = new char[cch];

	WideCharToMultiByte(cp, 0, pwsz, -1, psz, cch, NULL, NULL);

	std::string st(psz);
	delete[] psz;

	return st;
}

bool Expression::operator==(const Expression & rVal) const
{
	if (this->exp_id == rVal.exp_id) {
		return true;
	}
	else {
		return false;
	}
}

bool Expression::operator<(const Expression & rVal) const
{
	if (this->exp_id < rVal.exp_id) {
		return true;
	}
	else {
		return false;
	}
}

