#pragma once
#include "windows.h"
#include "resource.h"
#include <string>
class app
{
private:
	bool register_class(bool type);
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	
	HWND create_keyboard_window();
	HWND create_game_window();
	HMENU mMenuHandle;
	HINSTANCE m_instance;
	HWND m_main;
	HWND m_game;


public:
	app(HINSTANCE instance);
	int run(int show_command);
	static int index;
	static int timer;
};


