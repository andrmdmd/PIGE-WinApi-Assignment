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
	
	HINSTANCE m_instance;

	HWND create_keyboard_window();
	HWND create_game_window(int style, int size, int index);
	HMENU mMenuHandle;
	HWND m_main;
	HWND m_game[4];
	HWND overlay[4];
	HWND overlayWord[4];

	void DrawLetter(char c);
	void UpdateKey(char c, int ind);
	void UpdateBox(int col, int row);
	void AnimateBox(int col, int frame, int row);
	void DeleteLast();
	void ClearGame();
	void DrawNewGame();
	void GetKeyRects();
	void ChooseWords();
	void ReadWords();
	int GetDifficulty();
	void WriteDifficulty(int d);
	void DrawGameEnd(int i);
	void OnPaint(HWND hwnd);

public:
	static const int MAX_COLUMNS = 5;
	static const int MAX_ROWS = 10;
	static const int size = 55;
	const char keyboardLetters[27] = {' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M' };
	const int keyboardLayers[3] = { 10, 9, 7 };
	int currCol = 0;
	int currRow = 0;
	int boxState[MAX_COLUMNS][MAX_ROWS][4];
	int keyState[27][4];
	int colorBG[4];
	int setColor[4];

	static int boardsNum;

	RECT letterBox[MAX_COLUMNS][MAX_ROWS];
	RECT letterAnimation[MAX_COLUMNS][4];
	RECT letterKey[27];
	RECT keyColor[27][4];

	int index, frame;

	app(HINSTANCE instance);
	int run(int show_command);
	
};


