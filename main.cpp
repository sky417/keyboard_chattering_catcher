/**
 * 
 * main.cpp
 * 
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include "keyString.h"

using namespace std;

unordered_map<char, int> m;
int globalThreshold = 50;
char lastKey = 0x0;
unsigned __int64 lastTs = 0;

bool has_key(unordered_map<char, int> m, char key) {
    if (m.find(key) == m.end()) {
        return false;
    }

    return true;
}

bool has_key(unordered_map<int, string> m, char key) {
    if (m.find(key) == m.end()) {
        return false;
    }

    return true;
}

using time_point = std::chrono::system_clock::time_point;
std::string serializeTimePoint( const time_point& time, const std::string& format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    //std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );
    return ss.str();
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode == HC_ACTION)
    {
        unsigned __int64 now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        switch (wParam)
        {
        case WM_KEYDOWN:
            // if it is the same key
            // and the delta is less than threshold
            // ignore that keydown event
            unsigned int threshold;
            if (p->vkCode == lastKey) {
                if (has_key(m, lastKey)) {
                    threshold = m[lastKey];
                } else {
                    threshold = globalThreshold;
                }

                if ((now - lastTs) < threshold) {
                    cout << serializeTimePoint(chrono::system_clock::now(), "%Y-%m-%d %H:%M:%S")
                         << " ---- CHATTERING << " <<  keyStrMap[p->vkCode] << " >>, delta "  << (now - lastTs) << endl;
                    return 1;
                }
            }
            break;
        case WM_KEYUP:
            // keyup doesn't do much, we just need to record the timestamp and the key code
            lastTs = now;
            lastKey = p->vkCode;
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(void) {
    m[' '] = 100;
    m[0x8] = 30;

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
