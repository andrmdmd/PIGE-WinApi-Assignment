#include "app.h"
#include <stdexcept>
#include <time.h>
#include <windowsx.h>
#include <fstream>
#include <vector>
#include <wingdi.h>
#include <algorithm>
#include <random>
#include <string>

std::wstring const app::s_class_name{ L"Wordle" };
char currWord[5];
int app::boardsNum = 0;
int currCol = 0;
int currRow = 0;
int index = 0;
bool dragWindow = false;
int completed[4];
std::string word[4]; 
std::vector<std::string> wordList;

bool app::register_class()
{
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
	wcex.hbrBackground = CreateSolidBrush(RGB(255,255,255));
	//wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU);
	wcex.lpszClassName = s_class_name.c_str();
	wcex.hIconSm = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_WORDLE));

	return RegisterClassExW(&wcex) != 0;
}

HWND app::create_keyboard_window()
{
	HMENU menu = LoadMenu(m_instance, MAKEINTRESOURCEW(IDR_MENU));
	return CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"WORDLE - KEYBOARD",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX,
		GetSystemMetrics(SM_CXSCREEN) / 2 - 700 / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2,
		639,
		255,
		nullptr,
		menu,
		m_instance,
		this);
}

HWND app::create_game_window(int style, int size, int index)
{
	//keyColor[p][i].left = letterKey[p].left + (i % 2) * size / 2;
	//keyColor[p][i].top = letterKey[p].top + (i / 2) * size / 2;
	//keyColor[p][i].right = keyColor[p][i].left + size / 2;
	//keyColor[p][i].bottom = boardsNum == 4 ? keyColor[p][i].top + size / 2 : letterKey[p].bottom;
	int len2 = 333;
	int len3;
	int x_coord, y_coord;
	if (size == 1) {
		len3 = 423;
		x_coord = GetSystemMetrics(SM_CXSCREEN) / 2 - len2 / 2;
		y_coord = GetSystemMetrics(SM_CYSCREEN) / 2 - len3 / 2;
	}
	if (size == 2) {
		len3 = 543;
		x_coord = (GetSystemMetrics(SM_CXSCREEN) / 4 - len2 / 4) + (index) * (GetSystemMetrics(SM_CXSCREEN) / 2 - len2 / 2);
		y_coord = GetSystemMetrics(SM_CYSCREEN) / 2 - len3 / 2;
	}
	if(size == 4) {
		len3 = 603;
		x_coord = (GetSystemMetrics(SM_CXSCREEN) / 4 - len2 / 4) + (index % 2) * (GetSystemMetrics(SM_CXSCREEN) / 2 - len2 / 2);
		y_coord = (index/2) * (GetSystemMetrics(SM_CYSCREEN) / 4 - len3 / 4) + (index/2) * (GetSystemMetrics(SM_CYSCREEN) / 2 - len3 / 2);
	}
	int s = style == 1 ? WS_EX_LAYERED : 0;
	return CreateWindowExW(
		s,
		s_class_name.c_str(),
		L"WORDLE - PUZZLE",
		WS_OVERLAPPED | WS_VISIBLE | WS_EX_TOPMOST,
		x_coord,
		y_coord,
		len2, 
		len3,
		m_main, 
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
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_CHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_UNCHECKED);
			ClearGame();
			boardsNum = 1;
			WriteDifficulty(boardsNum);
			for (int i = 0; i < boardsNum; i++)
				m_game[i] = create_game_window(0, boardsNum, i);
			DrawNewGame();
			ChooseWords();
			break;

		case ID_DIFFICULTY_MEDIUM:
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_UNCHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_CHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_UNCHECKED);
			ClearGame();
			boardsNum = 2;
			WriteDifficulty(boardsNum);
			for (int i = 0; i < boardsNum; i++)
				m_game[i] = create_game_window(0, boardsNum, i);
			DrawNewGame();
			ChooseWords();
			break;

		case ID_DIFFICULTY_HARD:
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_EASY, MF_UNCHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
			CheckMenuItem(mMenuHandle, ID_DIFFICULTY_HARD, MF_CHECKED);
			ClearGame();
			boardsNum = 4;
			WriteDifficulty(boardsNum);
			for (int i = 0; i < boardsNum; i++)
				m_game[i] = create_game_window(0, boardsNum, i);
			DrawNewGame();
			ChooseWords();
			break;
		}
	}
	break;
	case WM_CLOSE:
		ClearGame();
		DestroyWindow(m_main);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;

	case WM_TIMER:
		if (wparam == 10) {
			UpdateBox(index++, currRow);
		}
		if (wparam == 15) {
			AnimateBox(index, frame++, currRow);
		}
		break;
	case WM_MOVE:
		for (int i = 0; i < boardsNum; i++) {
			if (window == overlay[i]) {
				if (colorBG[i] == 0) break;
				RECT rc;
				GetWindowRect(overlay[i], &rc);
				SetWindowPos(m_game[i], overlay[i], rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL);
			}
		}
		break;
	case WM_CHAR:
		if (window != m_main) break;

		switch (wparam) {
		case 0x08: 
			if (currCol == 0) break;
			DeleteLast();
			break;
		case 0x0D:
			if (currCol != 5) break;
			if (std::find(wordList.begin(), wordList.end(), currWord) == wordList.end()) {
				for (int i = 0; i < MAX_COLUMNS; i++) {
					DeleteLast();
				}
				break;
			}
			for (int k = 0; k < boardsNum; k++) {
				for (int i = 0; i < MAX_COLUMNS; i++) {
					for (int j = 0; j < MAX_COLUMNS; j++) {
						if (word[k][j] == currWord[i]) {
							if (i == j) {
								if (boxState[i][currRow][k] != 2) {
									boxState[i][currRow][k] = 2;
									if (completed[k] >= 5) continue;
									if(boxState[i][currRow-1][k]!= 2) completed[k]++;
								}
							}
							else if (boxState[i][currRow][k] != 2)
								boxState[i][currRow][k] = 1;
						}
					}
				}
			}
			GetKeyRects();
			for (int i = 0; i < MAX_COLUMNS; i++) {
				UpdateKey(currWord[i], i);
			}
			index = 0;
			for (int j = 0; j < boardsNum; j++) {
				for (int i = 0; i < MAX_COLUMNS; i++) {
					letterAnimation[i][j].left = letterBox[i][currRow].left;
					letterAnimation[i][j].right = letterBox[i][currRow].right;
					letterAnimation[i][j].top = letterBox[i][currRow].top + size/2 - 3;
					letterAnimation[i][j].bottom = letterBox[i][currRow].bottom - size/2 + 3;
				}
			}
			frame = 0;
			SetTimer(window, 15, 10, NULL);
			SetTimer(window, 10, 90, NULL);
			
			
			break;
		default:
			if (currCol == 5) break;

			if (currCol == 0)
				for (int i = 0; i < boardsNum; i++)
					if (completed[i] == 5)
						completed[i] = 6;

			char c = char(wparam);
			if (c >= 'A' && c <= 'Z') {
				DrawLetter(c);
				currWord[currCol] = c;
			}
				
			else if (c >= 'a' && c <= 'z') {
				DrawLetter(c + 'A' - 'a');
				currWord[currCol] = c + 'A' - 'a';
			}			
			else break;		
			currCol++;
		}
	case WM_NCHITTEST:
		if (window == m_main) break;
		return HTCAPTION;
	}
	return DefWindowProcW(window, message, wparam, lparam);

}

app::app(HINSTANCE instance) :m_instance{ instance }, m_main{}
{
	register_class();
	m_main = create_keyboard_window();
	//m_game[0] = create_game_window(1, 1);
	mMenuHandle = GetMenu(m_main);
	//boardsNum = 1;

	for (int i = 0; i < MAX_COLUMNS; i++) {
		for (int j = 0; j < MAX_ROWS; j++) {
			letterBox[i][j].left = 9 + i * (size + 6);
			letterBox[i][j].right = 9 + i * (size + 6) + size;
			letterBox[i][j].top = 10 + j * (size + 6);
			letterBox[i][j].bottom = 10 + j * (size + 6) + size;
			for(int k = 0; k < 4; k++)
				boxState[i][j][k] = 0;

		}
	}
	int p = 1, x_coord, y_coord;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < keyboardLayers[i]; j++) {

			x_coord = 9  + j * (size + 6);
			y_coord = 10 + i * (size + 6);
			if (i == 1) x_coord += 55 / 2 + 2;
			if (i == 2) x_coord += 3 * 55 / 2 + 8;
			letterKey[p].left = x_coord;
			letterKey[p].top = y_coord;
			letterKey[p].right = x_coord + size;
			letterKey[p].bottom = y_coord + size;
			p++;
			for(int k = 0; k < 4; k++)
				keyState[p][k] = 0;
		}
	}
}

int app::run(int show_command)
{
	SetWindowLong(m_main, GWL_EXSTYLE, GetWindowLong(m_main, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_main, 0, (255 * 80) / 100, LWA_ALPHA);
	ShowWindow(m_main, show_command);
	boardsNum = GetDifficulty();
	for (int i = 0; i < boardsNum; i++) {
		m_game[i] = create_game_window(0, boardsNum, i);
		colorBG[i] = 0;
		setColor[i] = 0;
	}
	DrawNewGame();
	GetKeyRects();
	ReadWords();
	ChooseWords();
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


void app::DrawNewGame() {

	HDC hdc;
	HPEN hPen, holdPen;
	HBRUSH hBrush, holdBrush;
	HFONT hFont, holdFont;

	for (int i = 0; i < boardsNum; i++) {

		hdc = GetDC(m_game[i]);

		hBrush = CreateSolidBrush(RGB(251, 252, 255));
		holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		hPen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
		holdPen = (HPEN)SelectObject(hdc, hPen);
		int size = 55;
		int rows = boardsNum == 1 ? 6 : boardsNum == 2 ? 8 : 10;
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < rows; j++)
				RoundRect(hdc, letterBox[i][j].left, letterBox[i][j].top, letterBox[i][j].right, letterBox[i][j].bottom, 5, 5);
		SelectObject(hdc, holdPen);
		DeleteObject(hPen);
		SelectObject(hdc, holdBrush);
		DeleteObject(hBrush);
		ReleaseDC(m_game[i], hdc);
	}

	hdc = GetDC(m_main);

	hBrush = CreateSolidBrush(RGB(251, 252, 255));
	holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
	holdPen = (HPEN)SelectObject(hdc, hPen);
	int p = 1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < keyboardLayers[i]; j++) {
			RoundRect(hdc, letterKey[p].left, letterKey[p].top, letterKey[p].right, letterKey[p].bottom, 5, 5);
			p++;
		}
	}

	hFont = CreateFontW(25, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
	holdFont = (HFONT)SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	for (int i = 1; i < 27; i++) {
		wchar_t* wString = new wchar_t[1];
		const char cc[2] = { keyboardLetters[i] };
		MultiByteToWideChar(CP_ACP, 0, cc, -1, wString, 1);
		DrawText(hdc, wString, 1, &letterKey[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	SelectObject(hdc, holdPen);
	DeleteObject(hPen);
	SelectObject(hdc, holdBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, holdFont);
	DeleteObject(hFont);
	ReleaseDC(m_main, hdc);
}

void app::GetKeyRects() {
	if (boardsNum == 1) return;
	for (int p = 1; p < 27; p++) {
		for (int i = 0; i < boardsNum; i++) {
			keyColor[p][i].left = letterKey[p].left + (i % 2) * size / 2;
			keyColor[p][i].top = letterKey[p].top + (i / 2) * size / 2;
			keyColor[p][i].right = keyColor[p][i].left + size / 2;
			keyColor[p][i].bottom = boardsNum == 4 ? keyColor[p][i].top + size / 2 : letterKey[p].bottom;
		}
	}
}

void app::DrawLetter(char c) {
	HDC hdc;
	HFONT hFont, holdFont;

	for (int i = 0; i < boardsNum; i++) {
		if (completed[i] > 5) continue;
		hdc = GetDC(m_game[i]);

		hFont = CreateFontW(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
		holdFont = (HFONT)SelectObject(hdc, hFont);

		wchar_t* wString = new wchar_t[1];
		const char cc[2] = { c };
		MultiByteToWideChar(CP_ACP, 0, cc, -1, wString, 1);

		DrawText(hdc, wString, 1, &letterBox[currCol][currRow], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(hdc, holdFont);
		DeleteObject(hFont);
		ReleaseDC(m_game[i], hdc);
	}
}

void app::UpdateKey(char c, int i){

	HDC hdc;
	PAINTSTRUCT ps;
	HFONT hFont, holdFont;
	HBRUSH hBrush, holdBrush; 
	HPEN hPen, holdPen;
	int p = 0;
	COLORREF c0 = RGB(164, 174, 196), c1 = RGB(243, 194, 55), c2 = RGB(121, 184, 81);
	hdc = GetDC(m_main);

	while (keyboardLetters[p] != c)
		p++;

	for (int k = 0; k < boardsNum; k++) {
		if (completed[k] > 5) continue;
		COLORREF color = boxState[i][currRow][k] == 2 ? c2 : boxState[i][currRow][k] == 1 ? c1 : c0;
		
		hBrush = CreateSolidBrush(color);
		holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		hPen = CreatePen(PS_SOLID, 2, color);
		holdPen = (HPEN)SelectObject(hdc, hPen);

		if (keyState[p][k] == 2) {}
		else if (keyState[p][k] == 1 && boxState[i][currRow][k] == 0) {}
		else {
			keyState[p][k] = boxState[i][currRow][k];
			if(boardsNum == 1)
				RoundRect(hdc, letterKey[p].left, letterKey[p].top, letterKey[p].right, letterKey[p].bottom, 5, 5);
			else {
				SetBkMode(hdc, TRANSPARENT);
				RoundRect(hdc, keyColor[p][k].left, keyColor[p][k].top, keyColor[p][k].right, keyColor[p][k].bottom, 5, 5);
			}

		}
		SelectObject(hdc, holdPen);
		DeleteObject(hPen);
		SelectObject(hdc, holdBrush);
		DeleteObject(hBrush);

	}

	hFont = CreateFontW(25, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
	holdFont = (HFONT)SelectObject(hdc, hFont);
	wchar_t* wString = new wchar_t[1];
	const char cc[2] = { c };
	MultiByteToWideChar(CP_ACP, 0, cc, -1, wString, 1);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, wString, 1, &letterKey[p], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(hdc, holdFont);
	DeleteObject(hFont);

	ReleaseDC(m_main, hdc);
}

void app::UpdateBox(int col, int row) {
	
	if (index == 5) {
		KillTimer(m_main, 10);
		currRow++;
		currCol = 0;
	}

	HDC hdc;
	HFONT hFont, holdFont;
	HBRUSH hBrush, holdBrush;
	HPEN hPen, holdPen;
	
	COLORREF c0 = RGB(164, 174, 196), c1 = RGB(243, 194, 55), c2 = RGB(121, 184, 81);

	for (int i = 0; i < boardsNum; i++) {
		if (completed[i] > 5) continue;
		COLORREF c = boxState[col][row][i] == 2 ? c2 : boxState[col][row][i] == 1 ? c1 : c0;
		//hdc = BeginPaint(m_game[i], &ps);
		hdc = GetDC(m_game[i]);
		hBrush = CreateSolidBrush(c);
		holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		hPen = CreatePen(PS_SOLID, 2, c);
		holdPen = (HPEN)SelectObject(hdc, hPen);
		int size = 55;
		int rows = boardsNum == 1 ? 6 : boardsNum == 2 ? 8 : 10;

		RoundRect(hdc, letterBox[col][row].left, letterBox[col][row].top, letterBox[col][row].right, letterBox[col][row].bottom, 5, 5);

		hFont = CreateFontW(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
		holdFont = (HFONT)SelectObject(hdc, hFont);
		wchar_t* wString = new wchar_t[1];
		const char cc[2] = { currWord[col] };
		MultiByteToWideChar(CP_ACP, 0, cc, -1, wString, 1);
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, wString, 1, &letterBox[col][row], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(hdc, holdPen);
		DeleteObject(hPen);
		SelectObject(hdc, holdBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, holdFont);
		DeleteObject(hFont);
		ReleaseDC(m_game[i], hdc);

			
	}

	if (index == 5) {
		index = 0;

		for (int i = 0; i < boardsNum; i++) {
			if (completed[i] >= 5 && colorBG[i] == 0) {
				colorBG[i] = 2;
				setColor[i] = 1;
			}
			else if (colorBG[i] == 0 && completed[i] < 4 && ((boardsNum == 1 && currRow == 6) || (boardsNum == 2 && currRow == 8) || currRow == 10)) {
				completed[i] = 6;
				colorBG[i] = 1;
				setColor[i] = 1;
			}
		}

		for (int i = 0; i < boardsNum; i++) {
			if (setColor[i] == 1) {
				setColor[i] = 0;
				DrawGameEnd(i);
			}
		}
	}
		

}

void app::DeleteLast() {
	HDC hdc;
	HBRUSH hBrush, holdBrush;
	HPEN hPen, holdPen;
	currCol--;
	for (int i = 0; i < boardsNum; i++) {
		hdc = GetDC(m_game[i]);

		hBrush = CreateSolidBrush((RGB(251, 252, 255)));
		holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		hPen = CreatePen(PS_SOLID, 2, RGB(222, 225, 233));
		holdPen = (HPEN)SelectObject(hdc, hPen);
		RoundRect(hdc, letterBox[currCol][currRow].left, letterBox[currCol][currRow].top, letterBox[currCol][currRow].right, letterBox[currCol][currRow].bottom, 5, 5);
		SelectObject(hdc, holdPen);
		DeleteObject(hPen);
		SelectObject(hdc, holdBrush);
		DeleteObject(hBrush);
		ReleaseDC(m_game[i], hdc);
	}
	
}

void app::ClearGame() {
	currCol = 0; currRow = 0;
	for (int k = 0; k < boardsNum; k++) {
		for (int i = 0; i < MAX_COLUMNS; i++) {
			for (int j = 0; j < MAX_ROWS; j++) {
				boxState[i][j][k] = 0;
			}
		}
		completed[k] = 0;
	}
	for (int i = 0; i < boardsNum; i++) {
		COLORREF color RGB(255, 255, 255);
		HBRUSH brush = CreateSolidBrush(color);
		SetClassLongPtr(m_game[i], GCLP_HBRBACKGROUND, (LONG_PTR)brush);
		DestroyWindow(overlay[i]);
		overlay[i] = nullptr;
		DestroyWindow(m_game[i]);
		m_game[i] = nullptr;
		colorBG[i] = 0;
	}
}

void app::ReadWords() {
	
	std::ifstream file("Wordle.txt");
	std::string w, w_mod;
	if (file.is_open()) {
		while (getline(file, w))
		{
			w_mod = w.substr(0, 6);
			std::transform(w_mod.begin(), w_mod.end(), w_mod.begin(), ::toupper); // https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case
			wordList.push_back(w_mod); 
		}
	}
	file.close();
}

void app::ChooseWords() {

	srand(time(0));

	for (int i = 0; i < boardsNum; i++) {
		std::string random = wordList[rand() % wordList.size()];
		word[i] = random;
	}

}

void app::WriteDifficulty(int d) {
	std::ofstream file("Wordle.ini");
	file << "[WORDLE]\n" << "DIFFICULTY=" << d;
	file.close();
}


int app::GetDifficulty() {
	std::ifstream file("Wordle.ini");
	char c; int d;
	while (file >> c) {
		if (c == '=') {
			file >> d;
			return d;
		}
			
	}
	return 1;
}

void app::DrawGameEnd(int i) {

	RECT rc;
	GetWindowRect(m_game[i], &rc);
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	overlay[i] = CreateWindowExW(
		WS_EX_LAYERED,
		s_class_name.c_str(),
		L"WORDLE - PUZZLE",
		WS_OVERLAPPED | WS_VISIBLE | WS_EX_TOPMOST,
		rc.left,
		rc.top,
		w,
		h,
		m_main,
		nullptr,
		m_instance,
		this);
	COLORREF color = colorBG[i] == 1 ? RGB(255, 0, 0) : RGB(0, 255, 0);
	HBRUSH brush = CreateSolidBrush(color);

	SetClassLongPtr(overlay[i], GCLP_HBRBACKGROUND, (LONG_PTR)brush);

	SetLayeredWindowAttributes(overlay[i], RGB(0, 255, 0), 128, LWA_ALPHA);
	
	if (colorBG[i] == 1) {
		GetClientRect(m_game[i], &rc);

		HDC hdc = GetDC(m_game[i]);

		HFONT hFont = CreateFontW(40, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
		HFONT holdFont = (HFONT)SelectObject(hdc, hFont);


		std::wstring str2(word[i].length(), L' '); 

		std::copy(word[i].begin(), word[i].end(), str2.begin());
		LPCWSTR str3 = str2.c_str();
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, RGB(255, 255, 255));
		DrawText(hdc, str3, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SelectObject(hdc, holdFont);
		DeleteObject(hFont);
		ReleaseDC(m_game[i], hdc);

		PAINTSTRUCT ps;
		hdc = BeginPaint(overlay[i], &ps);
		hFont = CreateFontW(40, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
		holdFont = (HFONT)SelectObject(hdc, hFont);


		std::wstring sstr2(word[i].length(), L' '); 

		std::copy(word[i].begin(), word[i].end(), sstr2.begin());
		LPCWSTR sstr3 = sstr2.c_str();
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, RGB(255, 255, 255));
		DrawText(hdc, sstr3, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SelectObject(hdc, holdFont);
		DeleteObject(hFont);
		EndPaint(overlay[i], &ps);
	}

}

void app::AnimateBox(int index, int frame, int row) {
	if (index == 4) {
		KillTimer(m_main, 15);
	}

	HDC hdc;
	HFONT hFont, holdFont;
	HBRUSH hBrush, holdBrush;
	HPEN hPen, holdPen;

	COLORREF c0 = RGB(164, 174, 196), c1 = RGB(243, 194, 55), c2 = RGB(121, 184, 81), cf0 = RGB(255,255,255);

	for (int i = 0; i < boardsNum; i++) {
		if (completed[i] > 5) continue;
		COLORREF c = boxState[index][row][i] == 2 ? c2 : boxState[index][row][i] == 1 ? c1 : c0;
		//hdc = BeginPaint(m_game[i], &ps);
		hdc = GetDC(m_game[i]);
		hBrush = CreateSolidBrush(c);
		holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		hPen = CreatePen(PS_SOLID, 2, c);
		holdPen = (HPEN)SelectObject(hdc, hPen);
		int size = 55;
		int rows = boardsNum == 1 ? 6 : boardsNum == 2 ? 8 : 10;

		RoundRect(hdc, letterAnimation[index][i].left, letterAnimation[index][i].top, letterAnimation[index][i].right, letterAnimation[index][i].bottom, 5, 5);
		letterAnimation[index][i].top -= 5;
		letterAnimation[index][i].bottom += 5;
		hFont = CreateFontW(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvatica");
		holdFont = (HFONT)SelectObject(hdc, hFont);
		wchar_t* wString = new wchar_t[1];
		const char cc[2] = { currWord[index] };
		MultiByteToWideChar(CP_ACP, 0, cc, -1, wString, 1);
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, wString, 1, &letterBox[index][row], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(hdc, holdPen);
		DeleteObject(hPen);
		SelectObject(hdc, holdBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, holdFont);
		DeleteObject(hFont);
		ReleaseDC(m_game[i], hdc);


	}
}