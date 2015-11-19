#pragma once

#include <windows.h>
#include <iostream>
#include <cmath>
#include <atlstr.h>
#include "GDIController.h"
#include "CanvasController.h"
#include "resource.h"
#include <windowsx.h>
#include "ExpParser.h"
#include "Expression.h"

void removeExpressionByIndex(int idx);

static bool paramChanged = true;
static double xPerGrid = 1.0;
static short zDelta;
static int offsetX = 0;
static int offsetY = 0;
static Expression ** exp_list;
static int exp_added = 0;


// Dialog global variables
static int exp_list_idx = -1;

// Drawing Function, execute when moving, scaling or adding new function
void draw(HWND hwnd)
{
	MathExpPlotter::CanvasController *canvasCtrl = nullptr;
	canvasCtrl = new MathExpPlotter::CanvasController(hwnd, 0, 0, 800, 600, 10, xPerGrid, 10, offsetX, offsetY);
	canvasCtrl->drawAxis(0x000000, 0xcccccc);

	for (auto i = 0; i < exp_added; ++i) {
		if (exp_list[i] == nullptr) {
			continue;
		}
		canvasCtrl->drawFromParser(
			exp_list[i]->exp_str,
			exp_list[i]->exp_color,
			exp_list[i]->rangeStart,
			exp_list[i]->rangeEnd
		);
	}
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

		int wmId, wmEvent;

		switch (msg) {
		case WM_SIZE:
		case WM_SETFOCUS:
		case WM_MOVE:
			draw(hwnd);
			paramChanged = false;
			break;
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
	
	BOOL CALLBACK SettingsDialogProc(
		HWND hwnd,      // 当前窗口句柄
		UINT Message,   // 收到的windows消息ID
		WPARAM wParam,  // 消息参数1
		LPARAM lParam)  // 消息参数2
	{
		int wmId, wmEvent;
		LPTSTR lbuf = new WCHAR[2048];
		HWND hList = ::GetDlgItem(hwnd, IDC_LIST_EXP);
		

		switch (Message) {
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// 低字段
			switch (wmId) {
			// 添加函数
			case IDC_EXP_ADD:
				::wsprintf(debug_buf, L"点击添加按钮。\n");
				::OutputDebugString(debug_buf);
				::GetDlgItemText(hwnd, IDC_EDIT_EXP, lbuf, 2048);
				::OutputDebugString(lbuf);
				exp_list[exp_added] = new Expression(std::string(CW2A(lbuf)), exp_added, 0x66ccff, -1000, 1000);
				exp_added += 1;
				paramChanged = true;
				// Send message to item list dialog
				::SendDlgItemMessage(hwnd, IDC_LIST_EXP, LB_ADDSTRING, 0, (LPARAM)(lbuf));
				break;
			case IDC_LIST_EXP:
				// IDC_EXP_LIST 高字段
				switch (wmEvent) {
				// 选择函数
				case LBN_SELCHANGE:
					exp_list_idx = ::SendMessage(hList, LB_GETCURSEL, 0, 0);
					::wsprintf(debug_buf, L"Label selection changed to %d.\n", exp_list_idx);
					::OutputDebugString(debug_buf);
					break;
				default:
					break;
				}
				break;
			case IDC_DELETE_EXP:
				// 删除指定表达式
				if (exp_list_idx < 0) {
					break;
				}
				else {
					removeExpressionByIndex(exp_list_idx);
					::SendMessage(hList, LB_DELETESTRING, exp_list_idx, NULL);
					paramChanged = false;
				}
				break;
			default:
				break;
			}
			
			break;
		}

		delete lbuf;
		return 0;
	}

	class WinMain
	{
	public:
		WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
			int nCmdShow, int windowWidth = 800, int windowHeight = 600):
			hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow), winWidth(windowWidth), winHeight(windowHeight)
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
				winWidth, winHeight,
				NULL, NULL, hInstance, NULL);
		}
		
		
		int CreateMainWindow()
		{
			if (hwnd == NULL) {
				::MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
				return -1;
			}

			/*
			hBtnOption = ::CreateWindow(TEXT("button"), TEXT("这是按钮"),
				WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT,
				winWidth - 200, winHeight - 100, 150, 50,
				hwnd, (HMENU)2, NULL, NULL
			);
			*/

			ShowWindow(hwnd, nCmdShow);
			
			HWND mainDialogHwnd = ::CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_SETTINGS), NULL, SettingsDialogProc);
			ShowWindow(mainDialogHwnd, nCmdShow);

			UpdateWindow(hwnd);
			UpdateWindow(hBtnOption);

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
		HWND       hwnd;
		HINSTANCE  hInstance;
		HINSTANCE  hPrevInstance;
		LPSTR      lpCmdLine;
		int        nCmdShow;
		MSG        msg;

		// GDI
		HDC         hdc;
		PAINTSTRUCT ps;
		HPEN        hpen;

		// Option Button
		HWND hBtnOption;

		int winHeight;
		int winWidth;
	};
}

void removeExpressionByIndex(int idx)
{
	int rm_idx = idx;
	int pass_idx_count = 0;
	for (auto i = 0; i < exp_added; i++) {
		if (exp_list[i] != nullptr) {
			pass_idx_count += 1;
			if (pass_idx_count == idx + 1) {
				rm_idx = i;
				break;
			}
		}
		else {
			continue;
		}
	}

	delete exp_list[rm_idx];
	exp_list[rm_idx] = nullptr;
	return;
}