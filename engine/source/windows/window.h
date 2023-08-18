#pragma once

#include <windows.h>

class Window
{
public:
	Window(HINSTANCE hInstance,
		   int posX,
		   int posY,
		   int width,
           int height);

	RECT GetSize();
	RECT GetClientAreaSize();

	void Show(bool show = true);

private:
	void InitWindowClass(HINSTANCE hInstance);
	void Init(HINSTANCE hInstance);

	static LRESULT CALLBACK WindowProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam);

public:
	int posX;
	int posY;

	int width;
	int height;

	HWND handle;
	WNDCLASSEX windowClass;
};