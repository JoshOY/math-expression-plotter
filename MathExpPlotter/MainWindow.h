#pragma once

#include <windows.h>
#include <cmath>
#include "GDIController.h"
#include "CanvasController.h"

double test_f_sin(double x)
{
	return 50 * std::sin(0.05 * x);
}

double test_f_cos(double x)
{
	return 50 * std::cos(0.05 * x);
}

double test_f_line(double x)
{
	return x;
}

namespace MathExpPlotter
{
	auto g_szClassName = L"Math Expression Plotter";

	LRESULT CALLBACK windowProcCallbackFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// GDIController *gdiCtrl = new GDIController(hwnd);
		// GDIController *gdiCtrl = nullptr;
		CanvasController *canvasCtrl = nullptr;

		switch (msg) {
		case WM_PAINT:
			// gdiCtrl->DrawCross();
			canvasCtrl = new CanvasController(hwnd, 0, 0, 800, 600, 10, 1.0, 5, 0, 0);
			canvasCtrl->drawAxis(0x000000, 0xcccccc);
			delete canvasCtrl;
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
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