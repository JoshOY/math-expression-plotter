#include "CanvasController.h"
#include <cstdio>
#include <tchar.h>

using namespace MathExpPlotter;

CanvasController::CanvasController(
	HWND _hwnd,
	int _canvasPositionTop,
	int _canvasPostionLeft,
	int _canvasWidth,
	int _canvasHeight,
	int _gridLength, // 10
	double _xPerGrid, // 1.0
	int _milesGap, // 5
	int _offsetX, // 0
	int _offsetY // 0
	): hwnd(_hwnd),
	canvasPosLeft(_canvasPostionLeft),
	canvasPosTop(_canvasPositionTop),
	canvasWidth(_canvasWidth),
	canvasHeight(_canvasHeight),
	gridLength(_gridLength),
	offsetX(_offsetX),
	offsetY(_offsetY),
	xPerGrid(_xPerGrid),
	milesGap(_milesGap)
{
	drawingFuncVec.clear();
	ps_ptr = nullptr;
	ps_ptr = new PAINTSTRUCT;

	hdc = ::GetDC(hwnd);

	hpen = ::CreatePen(PS_SOLID, 1, 0xFFFFFF);
	HBRUSH hbrush = CreateSolidBrush(0xFFFFFF);
	
	::SelectObject(hdc, hpen);
	::SelectObject(hdc, hbrush);

	::Rectangle(hdc, canvasPosLeft, canvasPosTop, canvasWidth, canvasHeight);

	::DeleteObject(hpen);
	::DeleteObject(hbrush);
}

CanvasController::~CanvasController()
{
	// ::EndPaint(hwnd, ps_ptr);
	::ReleaseDC(hwnd, hdc);
	if (ps_ptr != nullptr) {
		delete ps_ptr;
		ps_ptr = nullptr;
	}
}

void CanvasController::drawFunction(PlotFunc f, COLORREF lineColor)
{
	return drawFunction(f, lineColor, -99999999.0, 99999999.0);
}

void CanvasController::drawFunction(PlotFunc f, COLORREF lineColor, double rangeStart, double rangeEnd)
{
	if (rangeEnd < rangeStart) {
		std::fprintf(stderr, "[ERROR]: Range invalid.\n");
	}

	HPEN hpen = ::CreatePen(PS_SOLID, 1, lineColor);
	::SelectObject(hdc, hpen);

	int zeroPointX = ((canvasPosLeft + canvasWidth) / 2) + offsetX;
	int zeroPointY = ((canvasPosTop + canvasHeight) / 2) + offsetY;
	double x_per_pixel = static_cast<double>(this->xPerGrid) / static_cast<double>(this->gridLength);
	
	int drawing_pixel_x;
	double minimum_visible_x = static_cast<double>(-(zeroPointX - canvasPosLeft) * x_per_pixel);
	if (minimum_visible_x > rangeStart) {
		rangeStart = minimum_visible_x;
		drawing_pixel_x = canvasPosLeft - 1;
	}
	else {
		drawing_pixel_x = zeroPointX + static_cast<int>(rangeStart / x_per_pixel);
	}
	double maximum_visible_x = static_cast<double>((canvasPosLeft + canvasWidth - zeroPointX) * x_per_pixel);
	if (maximum_visible_x < rangeEnd)
	{
		rangeEnd = maximum_visible_x;
	}

	
	int startY = static_cast<int>(f(rangeStart));
	::MoveToEx(hdc, drawing_pixel_x, startY, NULL);

	for (double x = rangeStart; x < rangeEnd; x += x_per_pixel) {
		drawing_pixel_x += 1;
		double y = f(x);
		int drawing_y = static_cast<int>(-(y / x_per_pixel) + zeroPointY);
		/*if (drawing_y > canvasPosTop + canvasHeight || ) {
			break;
		}*/
		::LineTo(hdc, drawing_pixel_x, drawing_y);
	}

	::DeleteObject(hpen);
}


void CanvasController::drawFromParser(string exp, COLORREF lineColor, double rangeStart, double rangeEnd)
{
	if (rangeEnd < rangeStart) {
		std::fprintf(stderr, "[ERROR]: Range invalid.\n");
	}

	HPEN hpen = ::CreatePen(PS_SOLID, 1, lineColor);
	::SelectObject(hdc, hpen);

	int zeroPointX = ((canvasPosLeft + canvasWidth) / 2) + offsetX;
	int zeroPointY = ((canvasPosTop + canvasHeight) / 2) + offsetY;
	double x_per_pixel = static_cast<double>(this->xPerGrid) / static_cast<double>(this->gridLength);

	// Caculate min/max x value in canvas
	double canvasMinX = static_cast<double>(this->canvasPosLeft - zeroPointX) * x_per_pixel;
	double canvasMaxX = static_cast<double>(this->canvasWidth - zeroPointX) * x_per_pixel;
	double canvasMinY = static_cast<double>(zeroPointY - this->canvasPosTop - this->canvasHeight) * x_per_pixel;
	double canvasMaxY = static_cast<double>(zeroPointY) * x_per_pixel;

	// If the range has already out of range of canvas (< min)
	if (rangeEnd < canvasMinX) {
		::DeleteObject(hpen);
		return;
	}
	// If the range has already out of range of canvas (> max)
	if (rangeStart > canvasMaxX) {
		::DeleteObject(hpen);
		return;
	}

	// Get actual drawing x range.
	double real_x_max = (rangeEnd <= canvasMaxX) ? rangeEnd : canvasMaxX;
	double real_x_min = (rangeStart >= canvasMinX) ? rangeStart : canvasMinX;

	// Use parser to get.
	auto parser = ExpParser(exp);
	int drawing_width = static_cast<int>((real_x_max - real_x_min) / x_per_pixel);
	parser.calculateValueFromRange(real_x_min, real_x_max, drawing_width);

	// Draw the function.
	// int startDrawPointX = static_cast<int>((parser.get_x_at(0) - canvasMinX) / x_per_pixel);
	int px = zeroPointX + static_cast<int>(parser.get_x_at(0) / x_per_pixel);
	int py = zeroPointY - static_cast<int>(parser.get_y_at(0) / x_per_pixel);
	::MoveToEx(hdc, px, py, NULL);
	double x, y;

	int idx = 0;
	for (idx = 0; canvasMinX + (static_cast<double>(px) * x_per_pixel) <= real_x_max; ++idx) {
		x = parser.get_x_at(idx);
		y = parser.get_y_at(idx);
		if (x > canvasMaxX) {
			break;
		}
		px += 1;
		py = static_cast<int>(zeroPointY - static_cast<int>(y / x_per_pixel));
		::LineTo(hdc, px, py);
	}

	::DeleteObject(hpen);
}

void CanvasController::drawAxis(COLORREF colorAxis, // = 0x000000
							    COLORREF colorGrid) // = 0xCCCCCC
{
	// Calculate Variables

	int zeroPointX = ((canvasPosLeft + canvasWidth) / 2) + offsetX;
	int zeroPointY = ((canvasPosTop + canvasHeight) / 2) + offsetY;

	int milesGapNum_x_right = (canvasPosLeft + canvasWidth - zeroPointX) / (gridLength * milesGap);
	int milesGapNum_x_left  = (zeroPointX - canvasPosLeft) / (gridLength * milesGap);
	int milesGapNum_y_up    = (zeroPointY - canvasPosTop) / (gridLength * milesGap);
	int milesGapNum_y_down  = (canvasPosTop + canvasHeight - zeroPointY) / (gridLength * milesGap);

	int grid_num_horizontal = (canvasHeight / gridLength);
	int grid_num_vertical = (canvasWidth / gridLength);
	
	// Create Pens

	HPEN hpenAxis = ::CreatePen(PS_SOLID, 1, colorAxis);
	HPEN hpenGrid = ::CreatePen(PS_SOLID, 1, colorGrid);

	// Drawing the grids
	::SelectObject(hdc, hpenGrid);

	for (int i = 0; i < grid_num_horizontal; i++) {
		::MoveToEx(hdc, canvasPosLeft, canvasPosTop + i * gridLength + (offsetY % gridLength), NULL);
		::LineTo(hdc, canvasPosLeft + canvasWidth, canvasPosTop + i * gridLength + (offsetY % gridLength));
	}

	for (int i = 0; i < grid_num_vertical; i++) {
		::MoveToEx(hdc, canvasPosLeft + i * gridLength + (offsetX % gridLength), canvasPosTop, NULL);
		::LineTo(hdc, canvasPosLeft + i * gridLength + (offsetX % gridLength), canvasPosTop + canvasHeight);
	}

	::SelectObject(hdc, hpenAxis);

	// Draw X Axis
	::MoveToEx(hdc, zeroPointX, canvasPosTop, NULL);
	::LineTo(hdc, zeroPointX, canvasPosTop + canvasHeight);
	for (int i = 1; i <= milesGapNum_x_right; ++i) {
		::MoveToEx(hdc, zeroPointX + i * (gridLength * milesGap), zeroPointY - 5, NULL);
		::LineTo(hdc, zeroPointX + i * (gridLength * milesGap), zeroPointY + 5);

		TCHAR number_mark[100];
		_stprintf(number_mark, _T("%g"), i * milesGap * xPerGrid);
		::TextOut(hdc, zeroPointX + i * (gridLength * milesGap) - 2, zeroPointY + 5, number_mark, wcslen(number_mark));
	}
	for (int i = 1; i <= milesGapNum_x_left; ++i) {
		::MoveToEx(hdc, zeroPointX - i * (gridLength * milesGap), zeroPointY - 5, NULL);
		::LineTo(hdc, zeroPointX - i * (gridLength * milesGap), zeroPointY + 5);
	
		TCHAR number_mark[100];
		_stprintf(number_mark, _T("%g"), -i * milesGap * xPerGrid);
		::TextOut(hdc, zeroPointX - i * (gridLength * milesGap) - 2, zeroPointY + 5, number_mark, wcslen(number_mark));

	}

	// Draw Y Axis
	::MoveToEx(hdc, canvasPosLeft, zeroPointY, NULL);
	::LineTo(hdc, canvasPosLeft + canvasWidth, zeroPointY);
	for (int i = 1; i <= milesGapNum_y_down; ++i) {
		::MoveToEx(hdc, zeroPointX - 5, zeroPointY + i * (gridLength * milesGap), NULL);
		::LineTo(hdc, zeroPointX + 5, zeroPointY + i * (gridLength * milesGap));
	}
	for (int i = 1; i <= milesGapNum_y_up; ++i) {
		::MoveToEx(hdc, zeroPointX - 5, zeroPointY - i * (gridLength * milesGap),  NULL);
		::LineTo(hdc, zeroPointX + 5, zeroPointY - i * (gridLength * milesGap));
	}

	// Delete Pens
	::DeleteObject(hpenAxis);
	::DeleteObject(hpenGrid);
}