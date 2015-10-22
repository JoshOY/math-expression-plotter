#include "MathExp.h"

using namespace MathExpPlotter;

MathExp::MathExp(string math_exp_str)
{
	this->math_exp_str = math_exp_str;
	this->initialize();
}

MathExp::MathExp(LPCSTR _math_exp_lpcstr)
{
	this->math_exp_str = string(_math_exp_lpcstr);
	this->initialize();
}

void MathExp::initialize()
{
	op_set.insert('+');
	op_set.insert('-');
	op_set.insert('*');
	op_set.insert('/');
}


MathExp::~MathExp()
{
	// TODO
}
