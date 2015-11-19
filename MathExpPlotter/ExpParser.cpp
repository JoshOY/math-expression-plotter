#include "ExpParser.h"
#include <cstdlib>
#include <sstream>

using namespace std;
using namespace MathExpPlotter;

ExpParser::ExpParser()
{
	x_vec = new double[4096];
	y_vec = new double[4096];
}

ExpParser::ExpParser(const string s_init): mathexp_str(s_init)
{
	x_vec = new double[4096];
	y_vec = new double[4096];
}

ExpParser::~ExpParser()
{
	delete x_vec;
	delete y_vec;
}

void ExpParser::calculateValueFromRange(double x_start, double x_end, int stepNum)
{
	double x_step = (x_end - x_start) / (double)(stepNum);
	string s_input = "";
	char buf[2048];
	for (double x = x_start; x <= x_end; x += x_step) {
		sprintf_s(buf, "x=%g;", x);
		s_input += buf;
		s_input += this->mathexp_str + ";";
	}
	ofstream fout(".\\exp_in.txt");
	fout << s_input << std::endl;
	fout.close();
	// int ret = system(".\\parser.exe < .\\exp_in.txt > .\\exp_out.txt 2>.\\exp_log.txt");
	
	auto shellexec = ShellExecute(NULL,
		TEXT("OPEN"),
		TEXT("cmd.exe"),
		TEXT("/C .\\parser.exe < .\\exp_in.txt > .\\exp_out.txt 2>.\\exp_log.txt"),
		NULL, SW_HIDE);
	Sleep(50);

	ifstream fin(".\\exp_out.txt");
	double x_rdin = 0, y_rdin = 0;
	char buffer[256];
	this->step_total = stepNum;
	for (auto i = 0; i < stepNum; ++i) {
		fin >> buffer;
		sscanf(buffer, "x=%lf", &x_rdin);
		fin >> buffer;
		sscanf(buffer, "y=%lf", &y_rdin);
		this->x_vec[i] = x_rdin;
		this->y_vec[i] = y_rdin;
	}
}

double * ExpParser::get_y_vec()
{
	return this->y_vec;
}

double ExpParser::get_x_at(int idx)
{
	if (idx < 0 || idx >= this->step_total) {
		return 0;
	}
	else {
		return this->x_vec[idx];
	}
}

double ExpParser::get_y_at(int idx)
{
	if (idx < 0 || idx >= this->step_total) {
		return 0;
	}
	else {
		return this->y_vec[idx];
	}
}