#pragma once

#include <windows.h>
#include <iostream>
#include <cmath>
#include "GDIController.h"
#include "CanvasController.h"
#include <windowsx.h>


double test_f_sin(double x)
{
	return 50 * std::sin(0.05 * x);
}

double test_f_cos(double x)
{
	return 3.1415926 * std::cos(x);
}

double test_f_line(double x)
{
	return x;
}

double f_x_cube(double x)
{
	return x * x * x;
}

double f_1_x(double x) {
	/*if (x == 0.0) {
		return INT_MAX;
	}*/
	return 1.0 / x;
}

static bool paramChanged = true;
static double xPerGrid = 1.0;
static short zDelta;
static int offsetX = 0;
static int offsetY = 0;

void draw(HWND hwnd)
{
	MathExpPlotter::CanvasController *canvasCtrl = nullptr;
	canvasCtrl = new MathExpPlotter::CanvasController(hwnd, 0, 0, 800, 600, 10, xPerGrid, 10, offsetX, offsetY);
	canvasCtrl->drawAxis(0x000000, 0xcccccc);
	canvasCtrl->drawFunction(&test_f_cos, 0xFF00FF);
	canvasCtrl->drawFunction(&f_x_cube, 0x0000FF);
	canvasCtrl->drawFunction(&f_1_x, 0xF00FF0);
	delete canvasCtrl;
}



namespace MathExpPlotter
{
	auto g_szClassName = L"Math Expression Plotter";
	bool lButtonDown = false;
	int mouseX1, mouseY1, mouseX2, mouseY2;
	int dx = 0, dy = 0;
	WCHAR debug_buf[4096];

	LRESULT CALLBACK windowProcCallbackFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// GDIController *gdiCtrl = new GDIController(hwnd);
		// GDIController *gdiCtrl = nullptr;
		

		switch (msg) {
		case WM_PAINT:
			// gdiCtrl->DrawCross();
			if (paramChanged) {
				draw(hwnd);
				paramChanged = false;
			}
			break;
		case WM_LBUTTONDOWN:
			mouseX1 = GET_X_LPARAM(lParam);
			mouseY1 = GET_Y_LPARAM(lParam);
			lButtonDown = true;
			break;
		case WM_MOUSEMOVE:
			// draw(hwnd);
			if (lButtonDown) {
				mouseX2 = GET_X_LPARAM(lParam);
				mouseY2 = GET_Y_LPARAM(lParam);
				dx = mouseX2 - mouseX1;
				dy = mouseY2 - mouseY1;
				mouseX1 = mouseX2;
				mouseY1 = mouseY2;
				wsprintf(debug_buf, L"DEBUG: dx = %d, dy = %d;\n", dx, dy);
				OutputDebugString(debug_buf);

				offsetX += dx;
				offsetY += dy;
				draw(hwnd);
			}
			break;

		case WM_LBUTTONUP:
			lButtonDown = false;
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_MOUSEWHEEL:
			zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (zDelta < 0) {
				xPerGrid *= 1.25;
				if (xPerGrid > 0xFFFFFFFF) {
					xPerGrid /= 2;
				}
			}
			else {
				xPerGrid /= 1.25;
				if (xPerGrid <  0.00000000000000005) {
					xPerGrid *= 1.25;
				}
			}
			paramChanged = true;
			draw(hwnd);
			break;
		default:
			// delete gdiCtrl;
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}
	

	class WinMain
	{
	public:
		WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow):
			hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow)
		{
			auto wc = this->wc;
			wc.cbSize      = sizeof(WNDCLASSEX);
			wc.style       = 0;
			wc.lpfnWndProc = (MathExpPlotter::windowProcCallbackFunc);
			wc.cbClsExtra  = 0;
			wc.cbWndExtra  = 0;
			wc.hInstance   = hInstance;
			wc.hIcon = LoadIcon(NULL, IDC_ARROW);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = MathExpPlotter::g_szClassName;
			wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

			if (!RegisterClassEx(&wc)) {
				MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
				return;
			}

			this->hwnd = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				MathExpPlotter::g_szClassName,
				L"Math Expression Plotter",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				800, 600,
				NULL, NULL, hInstance, NULL);
		}
		
		
		int CreateMainWindow()
		{
			if (hwnd == NULL) {
				MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
				return -1;
			}

			ShowWindow(hwnd, nCmdShow);
			UpdateWindow(hwnd);

			return _messageLoop();
		}

		int _messageLoop()
		{
			while (GetMessage(&msg, NULL, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return msg.wParam;
		}

		int _hdcInit()
		{
			this->hdc = BeginPaint(this->hwnd, &ps);
			this->hpen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		}

		int _hdcDestory()
		{
			DeleteObject(hpen);
		}

	private:
		WNDCLASSEX wc;
		HWND hwnd;
		HINSTANCE hInstance;
		HINSTANCE hPrevInstance;
		LPSTR lpCmdLine;
		int nCmdShow;
		MSG msg;

		// GDI
		HDC hdc;
		PAINTSTRUCT ps;
		HPEN hpen;
	};
}