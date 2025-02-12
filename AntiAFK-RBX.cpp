#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <dwmapi.h>  

using namespace std::chrono_literals;

#define ID_TRAY_ICON     1
#define ID_EXIT          2
#define ID_START_AFK     3
#define ID_STOP_AFK      4
#define ID_SHOW_WINDOW   5
#define ID_HIDE_WINDOW   6
#define ID_INFORMATION   9
#define ID_EMULATE_SPACE 10
#define ID_MULTI_SUPPORT 11

HWND g_hwnd;
HINSTANCE g_hInst;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
std::atomic<bool> g_isAfkStarted(false), g_stopThread(false), g_multiSupport(false);
std::condition_variable g_cv;
std::mutex g_cv_m;
const TCHAR g_szClassName[] = _T("AntiAFK-RBX");
constexpr DWORD ACTION_DELAY = 30, ALT_DELAY = 15;
HANDLE g_hMultiInstanceMutex = NULL;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HICON CreateCustomIcon();

void ApplyDarkMode(HWND hWnd, bool enable)
{
    HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll");
    if (hUxTheme)
    {
        typedef int (WINAPI* SetPreferredAppModeFunc)(int);
        typedef void (WINAPI* FlushMenuThemesFunc)(void);
        SetPreferredAppModeFunc SetPreferredAppMode = (SetPreferredAppModeFunc)GetProcAddress(hUxTheme, (LPCSTR)135);
        FlushMenuThemesFunc FlushMenuThemes = (FlushMenuThemesFunc)GetProcAddress(hUxTheme, (LPCSTR)136);
        if (SetPreferredAppMode && FlushMenuThemes)
        {
            int mode = enable ? 2 : 0;
            SetPreferredAppMode(mode);
            FlushMenuThemes();
        }
    }
    HMODULE hDwm = LoadLibrary(L"dwmapi.dll");
    if (hDwm)
    {
        typedef HRESULT(WINAPI* DwmSetWindowAttributeFunc)(HWND, DWORD, LPCVOID, DWORD);
        DwmSetWindowAttributeFunc DwmSetWindowAttributePtr = (DwmSetWindowAttributeFunc)GetProcAddress(hDwm, "DwmSetWindowAttribute");
        if (DwmSetWindowAttributePtr)
        {
            const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
            BOOL dark = enable ? TRUE : FALSE;
            HRESULT hr = DwmSetWindowAttributePtr(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
            if (FAILED(hr))
            {
                const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_FALLBACK = 19;
                DwmSetWindowAttributePtr(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_FALLBACK, &dark, sizeof(dark));
            }
        }
        FreeLibrary(hDwm);
    }
}

struct EnumWindowsData {
    DWORD processId;
    bool includeHidden;
    std::vector<HWND>* wins;
};

BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lParam) {
    auto* data = reinterpret_cast<EnumWindowsData*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == data->processId && GetWindowTextLength(h) > 0 &&
        (data->includeHidden || IsWindowVisible(h)))
        data->wins->push_back(h);
    return TRUE;
}

void UpdateTrayIcon() {
    g_nid.hIcon = CreateCustomIcon();
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}

HICON CreateCustomIcon() {
    COLORREF innerColor = g_isAfkStarted.load() ? (g_multiSupport.load() ? RGB(0xC8, 0x00, 0x64) : RGB(0x33, 0x5F, 0xFF)) : RGB(0x17, 0x17, 0x17);
    BITMAPV5HEADER bi = { 0 };
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = 32;
    bi.bV5Height = -32;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;
    HDC screenDC = GetDC(NULL);
    void* lpBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(screenDC, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &lpBits, NULL, 0);
    ReleaseDC(NULL, screenDC);
    if (!hBitmap) return NULL;
    memset(lpBits, 0, 32 * 32 * 4);
    HDC memDC = CreateCompatibleDC(NULL);
    HGDIOBJ oldBmp = SelectObject(memDC, hBitmap);
    RECT rOuter = { 0, 0, 32, 32 };
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(memDC, &rOuter, hBrush);
    DeleteObject(hBrush);
    RECT rInner = { 2, 2, 30, 30 };
    hBrush = CreateSolidBrush(innerColor);
    FillRect(memDC, &rInner, hBrush);
    DeleteObject(hBrush);
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    HFONT hFont = CreateFont(23, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
    HFONT oldFont = (HFONT)SelectObject(memDC, hFont);
    DrawText(memDC, TEXT("A"), -1, &rInner, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(memDC, oldFont);
    DeleteObject(hFont);
    HBITMAP hMono = CreateBitmap(32, 32, 1, 1, NULL);
    ICONINFO ii = { 0 };
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = hMono;
    ii.hbmColor = hBitmap;
    HICON hIcon = CreateIconIndirect(&ii);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    DeleteObject(hBitmap);
    DeleteObject(hMono);
    return hIcon;
}

std::vector<HWND> GetWindowsForProcess(DWORD processId, bool includeHidden = false) {
    std::vector<HWND> wins;
    EnumWindowsData data{ processId, includeHidden, &wins };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return wins;
}

std::vector<HWND> FindAllRobloxWindows(bool includeHidden = false) {
    std::vector<HWND> wins;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return wins;
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0) {
                auto procWins = GetWindowsForProcess(pe.th32ProcessID, includeHidden);
                wins.insert(wins.end(), procWins.begin(), procWins.end());
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return wins;
}

HWND FindWindowByProcessName(const wchar_t* processName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return NULL;
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    HWND res = NULL;
    if (Process32First(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                auto procWins = GetWindowsForProcess(pe.th32ProcessID, true);
                if (!procWins.empty()) {
                    res = procWins.front();
                    break;
                }
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return res;
}

void ShowTrayNotification(const wchar_t* title, const wchar_t* msg) {
    NOTIFYICONDATA info = { 0 };
    info.cbSize = sizeof(NOTIFYICONDATA);
    info.hWnd = g_hwnd;
    info.uID = ID_TRAY_ICON;
    info.uFlags = NIF_INFO;
    lstrcpy(info.szInfoTitle, title);
    lstrcpy(info.szInfo, msg);
    info.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &info);
}

void ShowAllRobloxWindows_Multi() {
    auto wins = FindAllRobloxWindows(true);
    for (HWND w : wins) {
        if (!IsWindowVisible(w) || IsIconic(w)) {
            ShowWindow(w, SW_SHOW);
        }
    }
}

bool AdjustMemoryLimit(const wchar_t* windowName, SIZE_T min, SIZE_T max) {
    HWND rbx = FindWindow(NULL, windowName);
    if (!rbx) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(rbx, &pid);
    if (!pid) return false;
    HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, pid);
    if (!hProcess) return false;
    bool res = SetProcessWorkingSetSize(hProcess, min, max) != 0;
    CloseHandle(hProcess);
    return res;
}

void PressKey(WORD key) {
    keybd_event(static_cast<BYTE>(key), 0, 0, 0);
    keybd_event(static_cast<BYTE>(key), 0, KEYEVENTF_KEYUP, 0);
}

void PressSpace() {
    // keybd_event(static_cast<BYTE>(VK_MENU), static_cast<BYTE>(MapVirtualKey(VK_MENU, 0)), 0, 0);
    // Sleep(ALT_DELAY);
    keybd_event(static_cast<BYTE>(VK_SPACE), static_cast<BYTE>(MapVirtualKey(VK_SPACE, 0)), 0, 0);
    Sleep(ALT_DELAY);
    keybd_event(static_cast<BYTE>(VK_SPACE), static_cast<BYTE>(MapVirtualKey(VK_SPACE, 0)), KEYEVENTF_KEYUP, 0);
    // keybd_event(static_cast<BYTE>(VK_MENU), static_cast<BYTE>(MapVirtualKey(VK_MENU, 0)), KEYEVENTF_KEYUP, 0);
    // Sleep(ALT_DELAY);
    // keybd_event(static_cast<BYTE>(VK_MENU), static_cast<BYTE>(MapVirtualKey(VK_MENU, 0)), 0, 0);
    // keybd_event(static_cast<BYTE>(VK_MENU), static_cast<BYTE>(MapVirtualKey(VK_MENU, 0)), KEYEVENTF_KEYUP, 0);
}

void PerformJump(HWND target) {
    if (target) {
        SetForegroundWindow(target);
        Sleep(ACTION_DELAY);
        PressSpace();
        Sleep(ACTION_DELAY);
    }
}

void EnableMultiInstanceSupport() {
    if (!g_hMultiInstanceMutex)
        g_hMultiInstanceMutex = CreateMutex(NULL, TRUE, L"ROBLOX_singletonEvent");
}

void DisableMultiInstanceSupport() {
    if (g_hMultiInstanceMutex) {
        CloseHandle(g_hMultiInstanceMutex);
        g_hMultiInstanceMutex = NULL;
    }
}

void main_thread() {
    while (!g_stopThread.load()) {
        std::unique_lock<std::mutex> lock(g_cv_m);
        if (g_isAfkStarted.load()) {
            HWND user = GetForegroundWindow();
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty()) {
                ShowTrayNotification(L"AntiAFK Error", L"Roblox window not found!");
            }
            else {
                for (int i = 0; i < 3; i++) {
                    if (g_multiSupport.load()) {
                        for (HWND w : wins)
                            PerformJump(w);
                    }
                    else {
                        PerformJump(wins.front());
                    }
                    if (std::find(wins.begin(), wins.end(), GetForegroundWindow()) == wins.end())
                        SetForegroundWindow(user);
                    Sleep(ACTION_DELAY);
                }
            }
            g_cv.wait_for(lock, 10min, [] { return g_stopThread.load(); });
        }
        else {
            g_cv.wait_for(lock, 5000ms, [] { return g_stopThread.load(); });
        }
    }
}

void CreateTrayMenu(bool afk) {
    if (g_hMenu) DestroyMenu(g_hMenu);
    g_hMenu = CreatePopupMenu();
    AppendMenu(g_hMenu, MF_STRING, ID_INFORMATION, L"AntiAFK-RBX by Agzes");
    AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, L"Beta: v.2.0 | With ❤️");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_EMULATE_SPACE, L"Test Anti-AFK move");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
    AppendMenu(g_hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING | (g_multiSupport.load() ? MF_CHECKED : 0), ID_MULTI_SUPPORT, L"Multi-Instance Support");
    AppendMenu(g_hMenu, MF_STRING, ID_EXIT, L"Exit");
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        ApplyDarkMode(hwnd, true);
        g_nid.cbSize = sizeof(NOTIFYICONDATA);
        g_nid.hWnd = hwnd;
        g_nid.uID = ID_TRAY_ICON;
        g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_nid.uCallbackMessage = WM_USER + 1;
        g_nid.hIcon = CreateCustomIcon();
        lstrcpy(g_nid.szTip, L"v2.0 | AntiAFK-RBX");
        Shell_NotifyIcon(NIM_ADD, &g_nid);
        CreateTrayMenu(g_isAfkStarted.load());
        break;
    case WM_USER + 1:
        if (lParam == WM_RBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            ApplyDarkMode(hwnd, true);
            TrackPopupMenu(g_hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_INFORMATION: {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"GitHub link opened.");
            break;
        }
        case ID_START_AFK: {
            auto wins = FindAllRobloxWindows(true);
            if (!wins.empty()) {
                ShowTrayNotification(L"AntiAFK Started", L"AntiAFK has started.");
                g_isAfkStarted = true;
                CreateTrayMenu(true);
                UpdateTrayIcon();
            }
            else {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
            }
            break;
        }
        case ID_STOP_AFK:
            ShowTrayNotification(L"AntiAFK Stopped", L"AntiAFK has been stopped.");
            g_isAfkStarted = false;
            CreateTrayMenu(false);
            UpdateTrayIcon();
            break;
        case ID_SHOW_WINDOW: {
            if (!g_multiSupport.load()) {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx) {
                    ShowWindow(rbx, SW_SHOW);
                }
                else {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                }
            }
            else {
                ShowAllRobloxWindows_Multi();
            }
            break;
        }
        case ID_HIDE_WINDOW: {
            if (!g_multiSupport.load()) {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx) {
                    ShowWindow(rbx, SW_HIDE);
                }
                else {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                }
            }
            else {
                auto wins = FindAllRobloxWindows();
                for (HWND w : wins)
                    ShowWindow(w, SW_HIDE);
            }
            break;
        }
        case ID_EMULATE_SPACE: {
            auto wins = FindAllRobloxWindows(true);
            if (!wins.empty()) {
                if (g_multiSupport.load()) {
                    for (HWND w : wins) {
                        SetForegroundWindow(w);
                        Sleep(ACTION_DELAY);
                        PressSpace();
                    }
                }
                else {
                    SetForegroundWindow(wins.front());
                    Sleep(ACTION_DELAY);
                    PressSpace();
                }
            }
            else {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
            }
            break;
        }
        case ID_MULTI_SUPPORT:
            g_multiSupport = !g_multiSupport.load();
            if (g_multiSupport)
                EnableMultiInstanceSupport();
            else
                DisableMultiInstanceSupport();
            CreateTrayMenu(g_isAfkStarted.load());
            UpdateTrayIcon();
            break;
        case ID_EXIT:
            g_stopThread = true;
            g_cv.notify_all();
            Shell_NotifyIcon(NIM_DELETE, &g_nid);
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &g_nid);
        DestroyMenu(g_hMenu);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    g_hInst = hInstance;
    const wchar_t CLASS_NAME[] = L"AntiAFK-RBX-tray";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    RegisterClass(&wc);
    g_hwnd = CreateWindowEx(0, CLASS_NAME, L"AntiAFK-RBX-tray", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(g_hwnd, SW_HIDE);
    std::thread t(main_thread);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    {
        std::lock_guard<std::mutex> lock(g_cv_m);
        g_stopThread = true;
        g_cv.notify_all();
    }
    if (t.joinable()) t.join();
    return 0;
}
