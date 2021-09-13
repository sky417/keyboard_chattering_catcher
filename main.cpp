/**
 * 
 * main.cpp
 * 
 */

#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <windows.h>
#include <stdio.h>

using namespace std;

map<char, int> m;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            //cout << "Event " << getActionStr(wParam) << ", Hello " << p->vkCode << endl;
            unsigned __int64 now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            printf("%llu ::: Event 0x%04x, key 0x%x\n", now, wParam, p->vkCode);
            if (p->vkCode == 0x51)  {
                return 1;
            }
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(void) {
    m[' '] = 100;

    cout << "Hello world" << endl;

    // Install the low-level keyboard & mouse hooks
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    // Keep this app running until we're told to stop
    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);

    return 0;
}
