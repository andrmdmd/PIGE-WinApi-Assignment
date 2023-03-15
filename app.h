#pragma once
#include "windows.h"
#include "resource.h"
#include <string>
class app
{
private:
	bool register_class();
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
	HWND create_game_window(int size, int index);
	HMENU mMenuHandle;
	HINSTANCE m_instance;
	HWND m_main;
	HWND m_game[4];
	void DrawLetter(char c);
	void UpdateKey(char c, int ind);
	void UpdateBox(int col, int row);
	void DeleteLast();
	void ClearGame();
	void DrawNewGame();
	void GetKeyRects();
	void ChooseWords();
	void ReadWords();

public:
	static const int MAX_COLUMNS = 5;
	static const int MAX_ROWS = 10;
	static const int size = 55;
	const char keyboardLetters[27] = {' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M' };
	const int keyboardLayers[3] = { 10, 9, 7 };
	int currCol = 0;
	int currRow = 0;
	app(HINSTANCE instance);
	int run(int show_command);
	static int boardsNum;
	RECT letterBox[MAX_COLUMNS][MAX_ROWS];
	int boxState[MAX_COLUMNS][MAX_ROWS][4];
	int keyState[27][4];
	RECT letterKey[27];
	RECT keyColor[27][4];
	int index;
	
};


