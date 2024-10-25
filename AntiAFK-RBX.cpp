#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>

#define ID_TRAY_ICON 1
#define ID_EXIT 2
#define ID_START_AFK 3
#define ID_STOP_AFK 4
#define ID_SHOW_WINDOW 5
#define ID_HIDE_WINDOW 6
#define ID_INFORMATION 7
#define ID_EMULATE_SPACE 8
#define IDD_DIALOG 101
#define IDC_OK 102
#define IDC_GITHUB 103

HWND hwnd;
HINSTANCE hInst;
NOTIFYICONDATA nid;
HMENU hMenu;
bool isAfkStarted = false;
std::atomic<bool> stopThread(false);
std::condition_variable cv;
std::mutex cv_m;
const TCHAR szClassName[] = _T("AntiAFK-RBX");
const TCHAR szWindowTitle[] = _T("AntiAFK-RBX");

void CreateTrayMenu(bool isAfkStarted) {
    if (hMenu) {
        DestroyMenu(hMenu);
    }

    hMenu = CreatePopupMenu();

    AppendMenu(hMenu, MF_STRING, ID_EMULATE_SPACE, L"Test Anti-AFK move");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING | (isAfkStarted ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
    AppendMenu(hMenu, MF_STRING | (isAfkStarted ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
    AppendMenu(hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_INFORMATION, L"Information");
    AppendMenu(hMenu, MF_STRING, ID_EXIT, L"Exit");


}

HWND FindWindowByProcessName(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (hSnapshot == INVALID_HANDLE_VALUE) return NULL;
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                HWND hwnd = FindWindow(NULL, L"Roblox");
                CloseHandle(hSnapshot);
                return hwnd;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return NULL;
}

void MoveRobloxWindow(int x, int y, int width, int height) {
    HWND hwnd = FindWindowA(NULL, "Roblox");
    if (hwnd != NULL) {
        MoveWindow(hwnd, x, y, width, height, TRUE);
    }
    else {
        MessageBox(hwnd, L"Roblox window not found!", L"AntiAFK-RBX | Error", MB_OK);
    }
}
void PressKey(WORD key) {
    keybd_event(key, 0, 0, 0);
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}
void PressSpace() {
    keybd_event(VK_MENU, MapVirtualKey(VK_MENU, 0), 0, 0);
    Sleep(15);
    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, 0), 0, 0);
    Sleep(15);
    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, 0), KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, MapVirtualKey(VK_MENU, 0), KEYEVENTF_KEYUP, 0);
    Sleep(15);
    keybd_event(VK_MENU, MapVirtualKey(VK_MENU, 0), 0, 0);
    keybd_event(VK_MENU, MapVirtualKey(VK_MENU, 0), KEYEVENTF_KEYUP, 0);
}
void main_thread() {
    while (true) {
        if (isAfkStarted) {
            HWND userProgramHwnd = GetForegroundWindow();
            HWND robloxHwnd = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
            SetForegroundWindow(robloxHwnd);
            Sleep(11);
            PressSpace();
            Sleep(11);
            SetForegroundWindow(userProgramHwnd);
            Sleep(11);
            SetForegroundWindow(robloxHwnd);
            Sleep(11);
            PressSpace();
            Sleep(11);
            SetForegroundWindow(userProgramHwnd);
            Sleep(11);
            SetForegroundWindow(robloxHwnd);
            Sleep(11);
            PressSpace();
            Sleep(11);
            SetForegroundWindow(userProgramHwnd);
            std::unique_lock<std::mutex> lock(cv_m);
            if (cv.wait_for(lock, std::chrono::minutes(10), [] { return stopThread.load(); })) {
                break;
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

std::thread worker(main_thread);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = ID_TRAY_ICON;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER + 1;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        lstrcpy(nid.szTip, L"v1 | AntiAFK-RBX");
        Shell_NotifyIcon(NIM_ADD, &nid);
        CreateTrayMenu(isAfkStarted);
        break;

    case WM_USER + 1:
        if (lParam == WM_RBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_START_AFK: {
            HWND robloxHwnd = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
            if (robloxHwnd) {
                MessageBox(hwnd, L"Anti-AFK started!", L"AntiAFK-RBX | Information", MB_OK);
                isAfkStarted = true;
                CreateTrayMenu(isAfkStarted);
            }
            else {
                MessageBox(hwnd, L"Roblox window not found!", L"AntiAFK-RBX | Error", MB_OK);
            }
            break;
        }
        case ID_STOP_AFK:
            MessageBox(hwnd, L"Anti-AFK stoped!", L"AntiAFK-RBX | Information", MB_OK);
            isAfkStarted = false;
            CreateTrayMenu(isAfkStarted);
            break;
        case ID_SHOW_WINDOW: {
            HWND robloxHwnd = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
            ShowWindow(robloxHwnd, SW_SHOW);
            MoveRobloxWindow(100, 100, 800, 600);
            break;
        }
        case ID_HIDE_WINDOW: {
            HWND robloxHwnd = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
            ShowWindow(robloxHwnd, SW_HIDE);
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            MoveRobloxWindow(screenWidth, screenHeight, 800, 600);
            break;
        }
        case ID_EMULATE_SPACE: {
            HWND robloxHwnd = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
            SetForegroundWindow(robloxHwnd);
            Sleep(10);
            PressSpace();
            break;
        }
        case ID_INFORMATION:
            MessageBox(hwnd, L"AntiAFK-RBX by Agzes\nBeta 1.0\nhttps://github.com/Agzes/AntiAFK-RBX", L"AntiAFK-RBX | Information", MB_OK);
            break;
        case ID_EXIT:
            stopThread = true;
            cv.notify_all();
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyMenu(hMenu);
        PostQuitMessage(0);

        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
    hInst = hInstance;

    const wchar_t CLASS_NAME[] = L"AntiAFK-RBX-tray";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(0, CLASS_NAME, L"AntiAFK-RBX-tray", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    worker.join();
    return 0;
}
