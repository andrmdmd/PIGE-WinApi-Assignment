#include "app.h"
#include <stdexcept>
#include <time.h>

std::wstring const app::s_class_name{ L"Wordle" };

int app::index = 0;

bool app::register_class(bool type)
{
	//WNDCLASSEXW desc{};
	//if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
	//	return true;
	//desc = {
	//.cbSize = sizeof(WNDCLASSEXW),
	//.lpfnWndProc = window_proc_static,
	//.hInstance = m_instance,
	//.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	//.hbrBackground = CreateSolidBrush(RGB(255,255,255)),
	//.lpszClassName = s_class_name.c_str()
	//};
	WNDCLASSEXW wcex{};

	if (GetClassInfoExW(m_instance, s_class_name.c_str(), &wcex) != 0)
		return true;

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = window_proc_static;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_instance;
	wcex.hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_WORDLE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if(type == 1)
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU);
	wcex.lpszClassName = s_class_name.c_str();
	wcex.hIconSm = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_WORDLE));

	return RegisterClassExW(&wcex) != 0;
}

HWND app::create_keyboard_window()
{
	return CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"WORDLE - KEYBOARD",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		GetSystemMetrics(SM_CXSCREEN) / 2 - 700 / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2,
		700,
		300,
		nullptr,
		nullptr,
		m_instance,
		this);
}

HWND app::create_game_window()
{
	return CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"MyGame",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX,
		GetSystemMetrics(SM_CXSCREEN) / 4,
		GetSystemMetrics(SM_CYSCREEN) / 4,
		GetSystemMetrics(SM_CXSCREEN) / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2,
		nullptr,
		nullptr,
		m_instance,
		this);
}

LRESULT app::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app* app0 = nullptr;
	if (message == WM_NCCREATE)
	{
		app0 = static_cast<app*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app0));
	}
	else
		app0 = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));



	LRESULT res = app0 ?
		app0->window_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

LRESULT app::window_proc(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wparam);
		// Parse the menu selections:
		switch (wmId)
		{
			case ID_DIFFICULTY_EASY:
			{
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_CHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_UNCHECKED);
				break;
			}	
			
			case ID_DIFFICULTY_MEDIUM:
			{
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_UNCHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_CHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_UNCHECKED);
				break;
			}	
			
			case ID_DIFFICULTY_HARD:
			{
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_UNCHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
				CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_CHECKED);
				break;
			}
		}
	}
	break;
	case WM_CLOSE:
		DestroyWindow(m_main);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	case WM_CTLCOLORSTATIC:
		return reinterpret_cast<INT_PTR>(CreateSolidBrush(RGB(0, 0, 0)));
		break;

	}
	return DefWindowProcW(window, message, wparam, lparam);

}
app::app(HINSTANCE instance) :m_instance{ instance }, m_main{}
{
	register_class(1);
	m_main = create_keyboard_window();
	mMenuHandle = GetMenu(m_main);
}

int app::run(int show_command)
{
	SetWindowLong(m_main, GWL_EXSTYLE, GetWindowLong(m_main, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_main, 0, (255 * 80) / 100, LWA_ALPHA);
	ShowWindow(m_main, show_command);
	UpdateWindow(m_main);
	MSG msg{};
	BOOL result = TRUE;

	srand(time(NULL));

	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}


