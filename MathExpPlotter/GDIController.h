#pragma once

#include <windows.h>
#include <vector>

using std::vector;
typedef double(*MathFunc)(double x);

namespace MathExpPlotter
{
	struct Point
	{
		int x;
		int y;
	};

	class GDIController
	{
	public:
		GDIController(HWND hwnd, int space = 20, int winWidth = 800, int winHeight = 600,
			          int startPointX = 0, int startPointY = 0)
		{
			this->hwnd = hwnd;
			this->gridSpace = space;
			this->winWidth = winWidth;
			this->winHeight = winHeight;
			this->winStartPointX = startPointX;
			this->winStartPointY = startPointY;
			this->hdcptr = new HDC;
						
		}
		~GDIController()
		{
			SafeEndPaint();
			if (hdcptr != nullptr) {
				delete hdcptr;
			}
			if (!hPenDeleted) {
				DeleteObject(hPen);
			}
			if (ps != nullptr) {
				delete ps;
			}
		}

		void SafeCreatePen(int penStyle, int cWidth, COLORREF colorRef)
		{
			if (drawStarted == false) {
				// hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				this->hPen = CreatePen(penStyle, cWidth, colorRef);
				this->hPenDeleted = false;
				if (ps != nullptr)
				{
					delete ps;
					ps = nullptr;
				}
				ps = new PAINTSTRUCT;

				hdcptr = new HDC;
				*hdcptr = BeginPaint(hwnd, ps);
				this->drawStarted = true;
			}
			else {
				DeleteObject(hPen);
				hPen = CreatePen(penStyle, cWidth, colorRef);
			}

			SelectObject(*hdcptr, this->hPen);
		}

		void SafeEndPaint()
		{
			if (!hPenDeleted) {
				DeleteObject(hPen);
				hPen = nullptr;
				this->hPenDeleted = true;
			}
			
			if (drawStarted) {
				EndPaint(hwnd, ps);
				delete ps;
				ps = nullptr;
				drawStarted = false;
			}

			if (hdcptr != nullptr) {
				delete hdcptr;
				hdcptr = nullptr;
			}
		}

		void DrawCross(COLORREF color = RGB(0, 0, 0), COLORREF gridColor = RGB(176, 176, 176))
		{

			// Grey grid
			SafeCreatePen(PS_SOLID, 1, gridColor);
			// Rows
			for (int i = 0; i < (winHeight / this->gridSpace); i++) {
				MoveToEx(*hdcptr, winStartPointX, winStartPointY + gridSpace * i, NULL);
				LineTo(*hdcptr, winStartPointX + winWidth, winStartPointY + gridSpace * i);
			}
			// Columns
			for (int i = 0; i < (winWidth / this->gridSpace); i++) {
				MoveToEx(*hdcptr, winStartPointX + gridSpace * i, winStartPointY, NULL);
				LineTo(*hdcptr, winStartPointX + gridSpace * i, winStartPointY + winHeight);
			}

			// Cross
			SafeCreatePen(PS_SOLID, 1, color);

			// Axis X
			MoveToEx(*hdcptr, winStartPointX, winStartPointY + winHeight / 2, NULL);
			LineTo(*hdcptr, winStartPointX + winWidth, winStartPointY + winHeight / 2);
			// Axis Y
			MoveToEx(*hdcptr, winStartPointX + winWidth / 2, winStartPointY, NULL);
			LineTo(*hdcptr, winStartPointX + winWidth / 2, winStartPointY + winHeight);

		}

		void DrawFunction(MathFunc f, COLORREF color = RGB(0, 0, 0))
		{
			// DrawCross();
			
			SafeCreatePen(PS_SOLID, 1, color);

			vector<Point> pointSet = GetPointList(f);
			for (int x = 0; x < pointSet.size(); x++) {
				if (x == 0) {
					MoveToEx(*hdcptr, winStartPointX, winStartPointY + winHeight / 2 - pointSet[x].y, NULL);
				}
				LineTo(*hdcptr, winStartPointX + x, winStartPointY + winHeight / 2 - pointSet[x].y);
			}
			// SafeEndPaint();
		}

		vector<Point> GetPointList(MathFunc f)
		{
			vector<Point> pointVec;
			for (int i = -winWidth / 2; i < winWidth / 2; i++) {
				double x = double(i);
				pointVec.push_back(Point{int(x), int(f(x))});
			}
			return pointVec;
		}

		double test_exp(double x)
		{
			return x * x;
		}

	private:
		HWND hwnd;
		int winStartPointX;
		int winStartPointY;
		int winWidth;
		int winHeight;
		int gridSpace;

		HDC *hdcptr = nullptr;
		PAINTSTRUCT *ps = nullptr;
		HPEN hPen;
		bool hPenDeleted = true;
		bool drawStarted = false;
	};
}