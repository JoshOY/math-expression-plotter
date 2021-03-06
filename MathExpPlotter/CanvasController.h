#pragma once

#include <windows.h>
#include <vector>
#include "ExpParser.h"

using std::vector;
typedef double(*PlotFunc)(double);

namespace MathExpPlotter
{
	class CanvasController
	{
	public:
		CanvasController(
				HWND hwnd,
				int canvasPositionTop,
				int canvasPostionLeft,
				int canvasWidth,
				int canvasHeight,
				int gridLength,
				double _xPerGrid,
				int milesGap,
				int offsetX,
				int offsetY
			);
		CanvasController(HWND _hwnd);
		~CanvasController();
		void drawAxis(COLORREF colorAxis, COLORREF colorGrid);
		void drawFunction(PlotFunc f, COLORREF lineColor);
		void drawFunction(PlotFunc f, COLORREF lineColor, double rangeStart, double rangeEnd);
		void drawFromParser(string exp, COLORREF lineColor, double rangeStart, double rangeEnd);

	private:
		HWND hwnd;
		int canvasPosLeft;
		int canvasPosTop;
		int canvasWidth;
		int canvasHeight;
		int gridLength;
		int offsetX;
		int offsetY;
		int milesGap;
		double xPerGrid;

		vector<PlotFunc> drawingFuncVec;

		HDC hdc;
		PAINTSTRUCT *ps_ptr;
		HPEN hpen;
	};
}