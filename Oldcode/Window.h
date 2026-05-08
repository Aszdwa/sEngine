#pragma once
#include "Defines.h"
enum class WINDOW_STYLE {
	BORDERLESS,
	FULLSCREEN,
	WINDOWED
};
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
class Window {
public:
	// Public function prototypes
	int Create(void);
	void WindowEvents(void);
	void WaitMessages(void);
	void ShowWindow(void);
	// Helper functions
	inline bool IsRunning(void) const { return isRunning; };
	/*inline */HWND GetWindowHandle(void) const { return hwnd; };
	/*inline */HINSTANCE GetWindowInstance(void) const { return hInstance; };
	/*inline */uint32_t GetWidth(void) const { return width; };
	/*inline */uint32_t GetHeight(void) const { return height; };
	/*inline */void GetWindowArea(int& width, int& height);
	void SetFbResize(bool fb) { fbResize = &fb; };
	void SetWindowStyle(WINDOW_STYLE style);
private:
	// Window variables
	HWND hwnd;
	HINSTANCE hInstance;
	uint32_t width = 800, height = 600;
	MSG msg = {};
	Window* pWindow = nullptr;
	bool isRunning = false;
	RECT rect;
	// Window procedure
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool* fbResize;
};
#elif LINUX
// Linux code...
#endif