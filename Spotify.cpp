#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

// Initialize keyboard hook
HHOOK kHook;

// Define functions/threads so we can call it above.
DWORD WINAPI HookThread(LPVOID lParam);

// Media Commands for spotify
enum SPOTIFY
{
	SPOTIFY_MUTE = 524288,
	SPOTIFY_VOLUMEDOWN = 589824,
	SPOTIFY_VOLUMEUP = 655360,
	SPOTIFY_NEXT = 720896,
	SPOTIFY_PREV = 786432,
	SPOTIFY_STOP = 851968,
	SPOTIFY_PLAYPAUSE = 917504
};

int main()
{
	// Hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// Start global key hook in its own thread.
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)& HookThread, NULL, 0, 0);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

LRESULT CALLBACK CallBack(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT keyStruct = *((KBDLLHOOKSTRUCT*)lParam);

	// Get the handle for spotify
	HWND Window_Handle = FindWindow(L"Chrome_WidgetWin_0", NULL);

	if (nCode >= 0 && wParam == WM_KEYDOWN) {

		switch (keyStruct.vkCode)
		{
		case(32): // Space Bar
			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
				SendMessage(Window_Handle, WM_APPCOMMAND, 0, SPOTIFY_PLAYPAUSE);
			}
			break;

		case(37): // Left Arrow
			if(GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
				SendMessage(Window_Handle, WM_APPCOMMAND, 0, SPOTIFY_PREV);
			}
			break;

		case(38): // Up Arrow
			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
				SendMessage(Window_Handle, WM_APPCOMMAND, 0, SPOTIFY_VOLUMEUP);
			}
			break;

		case(39): // Right Arrow
			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
				SendMessage(Window_Handle, WM_APPCOMMAND, 0, SPOTIFY_NEXT);
			}
			break;

		case(40): // Down Arrow
			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
				SendMessage(Window_Handle, WM_APPCOMMAND, 0, SPOTIFY_VOLUMEDOWN);
			}
			break;

		default:
			break;
		}
	}
	return CallNextHookEx(kHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID lParam)
{
	kHook = SetWindowsHookEx(WH_KEYBOARD_LL, &CallBack, NULL, NULL);

	// HHOOK (kHook) returns NULL if the function fails.
	if(kHook == NULL) {
		MessageBox(NULL, L"Failed SetWindowsHookEx : 81 (Spotify.cpp)", L"Spotify Controls by Moobs#0001", MB_ICONEXCLAMATION | MB_OK);
	}
	else {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UnhookWindowsHookEx(kHook);
	}
	return 0;
}