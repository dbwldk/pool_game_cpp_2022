// poolProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "poolProject.h"

#include <stdlib.h>
#include <time.h>
#include <cmath>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_POOLPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POOLPROJECT));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POOLPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_POOLPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

//그라운드
RECT ground;
//공(기본 공)
RECT ball;

//global lparam
LPARAM g_lparam;

//global value
int g_x, g_y;

//click 좌표
int click_x, click_y;

//drop 좌표
int drop_x, drop_y;

//마우스 움직임 좌표
int move_x, move_y;

//기준 ball의 좌표
int std_x, std_y;

//기준 ball
double std_m; //기울기
int std_d; //이동거리

//press
bool g_press;

//스레드 함수
DWORD WINAPI user_F(LPVOID param)
{
	HDC hdc;
	HWND hWnd = (HWND)param;

	std_m = 0.0; //기울기
	std_d = 0; //이동 거리

	drop_x = LOWORD(g_lparam);
	drop_y = HIWORD(g_lparam);

	hdc = GetDC(hWnd);

	MoveToEx(hdc, std_x, std_y, NULL);

	std_m = (double)(std_y - drop_y) / (double)(std_x - drop_x); //기울기

	if (drop_x > std_x) {
		for (int i = std_x; i > ground.left; i--) {
			std_d++;
			g_x = i;
			g_y = std_m * (g_x - std_x) + std_y;

			/// 문맥 교환
			//Sleep(1);
			LineTo(hdc, g_x, g_y);

			//ground y 벗어나면 막기
			if ((g_y < ground.top)||(g_y > ground.bottom))
				break;
		}
	}
	else {
		for (int i = std_x; i < ground.right; i++) {
			g_x = i;
			g_y = std_m * (g_x - std_x) + std_y;

			/// 문맥 교환
			//Sleep(1);
			LineTo(hdc, g_x, g_y);

			//ground y 벗어나면 막기
			if ((g_y < ground.top) || (g_y > ground.bottom))
				break;
		}
	}
	ReleaseDC(hWnd, hdc);

	ExitThread(0);
	return 0;
}

// 스레드의 제어를 위해 선언
HANDLE g_hdl[3000] = { NULL, };
// 배열 첨자용 변수
int g_index = 0;
// 스레드의 ID 값 확인 선언
DWORD tid = 0;


//드래그로 큐대 구현
void cue(HDC hdc, HWND hWnd, LPARAM lParam) {
	//마우스가 클릭된 상태로 움직일 경우, 실시간으로 (클릭 시작 위치 => 마우스의 현재 위치)로 선분을 그린다.
	hdc = GetDC(hWnd);
	HPEN os_pen, my_pen;

	//펜 생성
	my_pen = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));
	//os로 전달
	os_pen = (HPEN)SelectObject(hdc, my_pen);

	move_x = LOWORD(lParam);
	move_y = HIWORD(lParam);

	MoveToEx(hdc, std_x, std_y, NULL);
	LineTo(hdc, move_x, move_y);

	//select
	SelectObject(hdc, os_pen);
	//delete
	DeleteObject(my_pen);

	ReleaseDC(hWnd, hdc);

	InvalidateRect(hWnd, NULL, TRUE); //배경 제외 화면 무효화
	Sleep(1);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; //hdc

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		g_press = true; //마우스 눌림
		click_x = LOWORD(lParam);
		click_y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		g_press = false; //마우스 안눌림

		///마우스가 떨어진 순간부터 공 이동

		tid = 0; //tid reset
		//마우스 좌표 넘기기
		g_lparam = lParam;

		//스레드 함수(user_F) 호출
		g_hdl[g_index++] = CreateThread(NULL, 0, user_F, hWnd, 0, NULL);

		// 스레드 생성 여부를 확인
		if (NULL == g_hdl)
		{
			MessageBox(hWnd, L"스레드 생성 실패", L"error", MB_OK);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		if (false == g_press)
			break; //마우스가 눌리지 않은 경우 동작 x

		hdc = GetDC(hWnd);
		cue(hdc, hWnd, lParam); //큐대
		
		break;

	case WM_RBUTTONDOWN:
	{
		int i = 0;
		for (i = 0; i < 3000; i++)
			SuspendThread(g_hdl[i]);
	}
	break;

	case WM_CREATE:
	{
		// 기본 공의 초기 좌표를 설정
		ball.left = 500;
		ball.top = 300;
		ball.right = 530;
		ball.bottom = 330;

		//ball 좌표 넘기기
		std_x = round((ball.left + ball.right) / 2);
		std_y = round((ball.top + ball.bottom) / 2);

		// 그라운드 좌표
		ground.left = 10;
		ground.top = 10;
		ground.right = 1000;
		ground.bottom = 600;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

		// 그라운드
		Rectangle(hdc, ground.left, ground.top, ground.right, ground.bottom);

		// 공
		Ellipse(hdc, ball.left, ball.top, ball.right, ball.bottom);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}