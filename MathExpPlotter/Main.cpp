#include "MainWindow.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	auto winMain = new MathExpPlotter::WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	auto ret = winMain->CreateMainWindow();
	delete winMain;
	return ret;
}