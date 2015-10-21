#include "CanvasController.h"

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

	hdc = ::BeginPaint(hwnd, ps_ptr);
}

CanvasController::~CanvasController()
{
	::EndPaint(hwnd, ps_ptr);
	if (ps_ptr != nullptr) {
		delete ps_ptr;
		ps_ptr = nullptr;
	}
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

	
	// Create Pens

	HPEN hpenAxis = ::CreatePen(PS_SOLID, 1, colorAxis);
	HPEN hpenGrid = ::CreatePen(PS_SOLID, 1, colorGrid);

	::SelectObject(hdc, hpenAxis);

	// Draw X Axis
	::MoveToEx(hdc, zeroPointX, canvasPosTop, NULL);
	::LineTo(hdc, zeroPointX, canvasPosTop + canvasHeight);
	for (int i = 1; i <= milesGapNum_x_right; ++i) {
		::MoveToEx(hdc, zeroPointX + i * (gridLength * milesGap), zeroPointY - 5, NULL);
		::LineTo(hdc, zeroPointX + i * (gridLength * milesGap), zeroPointY + 5);
	}
	for (int i = 1; i <= milesGapNum_x_left; ++i) {
		::MoveToEx(hdc, zeroPointX - i * (gridLength * milesGap), zeroPointY - 5, NULL);
		::LineTo(hdc, zeroPointX - i * (gridLength * milesGap), zeroPointY + 5);
	}

	// Draw Y Axis
	::MoveToEx(hdc, canvasPosLeft, zeroPointY, NULL);
	::LineTo(hdc, canvasPosLeft + canvasWidth, zeroPointY);

	// Delete Pens
	::DeleteObject(hpenAxis);
	::DeleteObject(hpenGrid);
}