// AntiAFK-RBX.cpp | The program for AntiAFK and Multi-Instance in Roblox. Or just Roblox Anti-AFK. | By Agzes
// https://github.com/Agzes/AntiAFK-RBX

int currentVersion = 4;

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
#include <winreg.h>
#include <WinInet.h>

#pragma comment(lib, "WinInet.lib")

using namespace std::chrono_literals;

#define ID_TRAY_ICON 1
#define ID_EXIT 2
#define ID_START_AFK 3
#define ID_STOP_AFK 4
#define ID_SHOW_WINDOW 5
#define ID_HIDE_WINDOW 6
#define ID_INFORMATION 9
#define ID_EMULATE_SPACE 10
#define ID_MULTI_SUPPORT 11
#define ID_FISHSTRAP_SUP 12
#define ID_UPDATE_AVAILABLE 1000
#define ID_ANNOUNCEMENT_TEXT 1001

#define ID_SETTINGS_MENU 30
#define ID_AUTO_UPDATE 32
#define ID_USER_SAFE 33

#define ID_TIME_SUBMENU 13
#define ID_ACTION_SUBMENU 14
#define ID_TIME_3 15
#define ID_TIME_6 16
#define ID_TIME_9 17
#define ID_TIME_11 18
#define ID_TIME_13 19
#define ID_TIME_15 20
#define ID_TIME_18 21
#define ID_ACTION_SPACE 22
#define ID_ACTION_WS 23
#define ID_ACTION_ZOOM 24
#define ID_TIME_5_SEC 25
#define ID_TIME_20_SEC 26

#define ID_LINKWIKI 201
#define ID_LINKGITHUB 202
#define ID_LINKSF  203
#define ID_LINKTTU 204

// Dev Mode -> added 5sec and 20sec to interval | can be turned on in code or in regedit
std::atomic<bool> g_devMode(false);
HWND g_hwnd;
HINSTANCE g_hInst;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
std::atomic<bool> g_isAfkStarted(false), g_stopThread(false), g_multiSupport(false), g_fishstrapSupport(false), g_autoUpdate(true), g_userSafe(false), g_updateFound(false);
std::condition_variable g_cv;
std::mutex g_cv_m;
const TCHAR g_szClassName[] = _T("AntiAFK-RBX");
constexpr DWORD ACTION_DELAY = 30, ALT_DELAY = 15;
int g_selectedTime = 540;
int g_selectedAction = 0; // 0 - space, 1 - w&s, 2 - zoom
HANDLE g_hMultiInstanceMutex = NULL;
std::atomic<bool> g_userActive(false); // User-Safe mode
std::atomic<uint64_t> g_lastActivityTime(0);
const int USER_INACTIVITY_WAIT = 3;
const int MAX_WAIT_TIME = 60;
std::thread g_activityMonitorThread;
std::atomic<bool> g_monitorThreadRunning(false);
std::atomic<bool> g_updateInterval(false);
bool announcement_isEnabled = false;
bool announcement_isWithNotify = false;
std::string announcementText;
std::string announcementLabel;
std::string announcementLink;
std::string announcementID;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HICON CreateCustomIcon();

void ApplyDarkMode(HWND hWnd, bool enable)
{
    HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll");
    if (hUxTheme)
    {
        typedef int(WINAPI * SetPreferredAppModeFunc)(int);
        typedef void(WINAPI * FlushMenuThemesFunc)(void);
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
        typedef HRESULT(WINAPI * DwmSetWindowAttributeFunc)(HWND, DWORD, LPCVOID, DWORD);
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

void ShowTrayNotification(const wchar_t *title, const wchar_t *msg)
{
    NOTIFYICONDATA info = {0};
    info.cbSize = sizeof(NOTIFYICONDATA);
    info.hWnd = g_hwnd;
    info.uID = ID_TRAY_ICON;
    info.uFlags = NIF_INFO;
    lstrcpy(info.szInfoTitle, title);
    lstrcpy(info.szInfo, msg);
    info.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &info);
}

struct EnumWindowsData
{
    DWORD processId;
    bool includeHidden;
    std::vector<HWND> *wins;
};

BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lParam)
{
    auto *data = reinterpret_cast<EnumWindowsData *>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == data->processId && GetWindowTextLength(h) > 0 &&
        (data->includeHidden || IsWindowVisible(h)))
        data->wins->push_back(h);
    return TRUE;
}

void UpdateTrayIcon()
{
    g_nid.hIcon = CreateCustomIcon();
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}

HICON CreateCustomIcon()
{
    COLORREF innerColor = g_isAfkStarted.load() ? (g_multiSupport.load() ? RGB(0xC8, 0x00, 0x64) : RGB(0x33, 0x5F, 0xFF)) : RGB(0x17, 0x17, 0x17);
    BITMAPV5HEADER bi = {0};
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
    void *lpBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(screenDC, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS, &lpBits, NULL, 0);
    ReleaseDC(NULL, screenDC);
    if (!hBitmap)
        return NULL;
    memset(lpBits, 0, 32 * 32 * 4);
    HDC memDC = CreateCompatibleDC(NULL);
    HGDIOBJ oldBmp = SelectObject(memDC, hBitmap);
    RECT rOuter = {0, 0, 32, 32};
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(memDC, &rOuter, hBrush);
    DeleteObject(hBrush);
    RECT rInner = {2, 2, 30, 30};
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
    ICONINFO ii = {0};
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

std::vector<HWND> GetWindowsForProcess(DWORD processId, bool includeHidden = false)
{
    std::vector<HWND> wins;
    EnumWindowsData data{processId, includeHidden, &wins};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return wins;
}

std::vector<HWND> FindAllRobloxWindows(bool includeHidden = false)
{
    std::vector<HWND> wins;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return wins;
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snap, &pe))
    {
        do
        {
            if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0 || (g_fishstrapSupport.load() && _wcsicmp(pe.szExeFile, L"eurotrucks2.exe") == 0))
            {
                auto procWins = GetWindowsForProcess(pe.th32ProcessID, includeHidden);
                wins.insert(wins.end(), procWins.begin(), procWins.end());
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return wins;
}

HWND FindWindowByProcessName(const wchar_t *processName)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return NULL;
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);
    HWND res = NULL;
    if (Process32First(snap, &pe))
    {
        do
        {
            if (_wcsicmp(pe.szExeFile, processName) == 0)
            {
                auto procWins = GetWindowsForProcess(pe.th32ProcessID, true);
                if (!procWins.empty())
                {
                    res = procWins.front();
                    break;
                }
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return res;
}

void RestoreForegroundWindow(HWND prevWnd) {
    if (!prevWnd) return;
    TCHAR className[256];
    GetClassName(prevWnd, className, 256);
    if (_tcscmp(className, L"AntiAFK-RBX-tray") == 0) return;
    if (!IsWindowVisible(prevWnd) || IsIconic(prevWnd)) return;
    ShowWindow(prevWnd, SW_SHOW);
    SetWindowPos(prevWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD prevThreadId = GetWindowThreadProcessId(prevWnd, NULL);
    AttachThreadInput(currentThreadId, prevThreadId, TRUE);
    BringWindowToTop(prevWnd);
    SetForegroundWindow(prevWnd);
    AttachThreadInput(currentThreadId, prevThreadId, FALSE);
    SetWindowPos(prevWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void ShowAllRobloxWindows_Multi()
{
    auto wins = FindAllRobloxWindows(true);
    for (HWND w : wins)
    {
        if (!IsWindowVisible(w) || IsIconic(w))
        {
            ShowWindow(w, SW_SHOW);
        }
    }
}

void AntiAFK_Action(HWND target)
{
    switch (g_selectedAction)
    {
    case 0: // Space
        keybd_event(static_cast<BYTE>(VK_SPACE), static_cast<BYTE>(MapVirtualKey(VK_SPACE, 0)), 0, 0);
        Sleep(ALT_DELAY);
        keybd_event(static_cast<BYTE>(VK_SPACE), static_cast<BYTE>(MapVirtualKey(VK_SPACE, 0)), KEYEVENTF_KEYUP, 0);
        break;
    case 1: // W & S
        keybd_event('W', MapVirtualKey('W', 0), 0, 0);
        Sleep(ALT_DELAY);
        keybd_event('W', MapVirtualKey('W', 0), KEYEVENTF_KEYUP, 0);
        Sleep(ALT_DELAY);
        keybd_event('S', MapVirtualKey('S', 0), 0, 0);
        Sleep(ALT_DELAY);
        keybd_event('S', MapVirtualKey('S', 0), KEYEVENTF_KEYUP, 0);
        break;
    case 2: // Zoom
        keybd_event('I', MapVirtualKey('I', 0), 0, 0);
        Sleep(ALT_DELAY);
        keybd_event('I', MapVirtualKey('I', 0), KEYEVENTF_KEYUP, 0);
        Sleep(ALT_DELAY);
        keybd_event('O', MapVirtualKey('O', 0), 0, 0);
        Sleep(ALT_DELAY);
        keybd_event('O', MapVirtualKey('O', 0), KEYEVENTF_KEYUP, 0);
        break;
    }
}

void SaveSettings()
{
    HKEY hKey;
    DWORD multiSupport = g_multiSupport.load();
    DWORD fishstrapSupport = g_fishstrapSupport.load();
    DWORD autoUpdate = g_autoUpdate.load();
    DWORD userSafe = g_userSafe.load();

    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, L"MultiSupport", 0, REG_DWORD, (const BYTE *)&multiSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"FishstrapSupport", 0, REG_DWORD, (const BYTE *)&fishstrapSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedTime", 0, REG_DWORD, (const BYTE *)&g_selectedTime, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedAction", 0, REG_DWORD, (const BYTE *)&g_selectedAction, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoUpdate", 0, REG_DWORD, (const BYTE *)&autoUpdate, sizeof(DWORD));
        RegSetValueEx(hKey, L"UserSafe", 0, REG_DWORD, (const BYTE *)&userSafe, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

void LoadSettings()
{
    HKEY hKey;
    DWORD multiSupport = 0, fishstrapSupport = 0, selectedTime = 10, selectedAction = 0, devMode = 0;
    DWORD autoUpdate = 1, userSafe = 0, autoLimitRam = 0; 
    DWORD dataSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey, L"MultiSupport", NULL, NULL, (LPBYTE)&multiSupport, &dataSize);
        RegQueryValueEx(hKey, L"FishstrapSupport", NULL, NULL, (LPBYTE)&fishstrapSupport, &dataSize);
        RegQueryValueEx(hKey, L"SelectedTime", NULL, NULL, (LPBYTE)&selectedTime, &dataSize);
        RegQueryValueEx(hKey, L"SelectedAction", NULL, NULL, (LPBYTE)&selectedAction, &dataSize);
        RegQueryValueEx(hKey, L"DevMode", NULL, NULL, (LPBYTE)&devMode, &dataSize); // DevMode
        RegQueryValueEx(hKey, L"AutoUpdate", NULL, NULL, (LPBYTE)&autoUpdate, &dataSize);
        RegQueryValueEx(hKey, L"UserSafe", NULL, NULL, (LPBYTE)&userSafe, &dataSize);
        RegCloseKey(hKey);
    }

    g_multiSupport = multiSupport;
    g_fishstrapSupport = fishstrapSupport;
    g_selectedTime = selectedTime;
    g_selectedAction = selectedAction;
    g_devMode = devMode; // DevMode
    g_autoUpdate = autoUpdate;
    g_userSafe = userSafe;
}

void EnableMultiInstanceSupport()
{
    if (!g_hMultiInstanceMutex)
        g_hMultiInstanceMutex = CreateMutex(NULL, TRUE, L"ROBLOX_singletonEvent");
}

void DisableMultiInstanceSupport()
{
    if (g_hMultiInstanceMutex)
    {
        CloseHandle(g_hMultiInstanceMutex);
        g_hMultiInstanceMutex = NULL;
    }
}

void CreateTrayMenu(bool afk)
{
    if (g_hMenu)
        DestroyMenu(g_hMenu);
    g_hMenu = CreatePopupMenu();

    HMENU InfoMenu = CreatePopupMenu();
    AppendMenu(InfoMenu, MF_STRING, ID_LINKTTU, L"[Link] - TipsToUse");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKWIKI, L"[Link] - Wiki");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKGITHUB, L"[Link] - GitHub");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKSF, L"[Link] - SourceForge");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)InfoMenu, L"AntiAFK-RBX by Agzes");

    AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, L"Beta: v.2.2.1 | With ❤️");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_EMULATE_SPACE, L"Test Anti-AFK move");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);

    HMENU hTimeSubmenu = CreatePopupMenu();
    if (g_devMode.load())
    {
        AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 5 ? MF_CHECKED : 0), ID_TIME_5_SEC, L"5 seconds [dev]");
        AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 20 ? MF_CHECKED : 0), ID_TIME_20_SEC, L"20 seconds [dev]");
    }
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 3 ? MF_CHECKED : 0), ID_TIME_3, L"3 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 6 ? MF_CHECKED : 0), ID_TIME_6, L"6 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 9 ? MF_CHECKED : 0), ID_TIME_9, L"9 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 11 ? MF_CHECKED : 0), ID_TIME_11, L"11 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 13 ? MF_CHECKED : 0), ID_TIME_13, L"13 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 15 ? MF_CHECKED : 0), ID_TIME_15, L"15 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 18 ? MF_CHECKED : 0), ID_TIME_18, L"18 minutes");
    wchar_t timeLabel[32];
    if (g_selectedTime < 60)
        swprintf_s(timeLabel, L"Set Interval • %d sec", g_selectedTime);
    else
        swprintf_s(timeLabel, L"Set Interval • %d min", g_selectedTime / 60);
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hTimeSubmenu, timeLabel);

    HMENU hActionSubmenu = CreatePopupMenu();
    AppendMenu(hActionSubmenu, MF_STRING | (g_selectedAction == 0 ? MF_CHECKED : 0), ID_ACTION_SPACE, L"Space (Jump)");
    AppendMenu(hActionSubmenu, MF_STRING | (g_selectedAction == 1 ? MF_CHECKED : 0), ID_ACTION_WS, L"W/S");
    AppendMenu(hActionSubmenu, MF_STRING | (g_selectedAction == 2 ? MF_CHECKED : 0), ID_ACTION_ZOOM, L"Zoom (I/O)");
    wchar_t actionLabel[32];
    const wchar_t* actionNames[] = { L"Space (Jump)", L"W/S", L"Zoom (I/O)" };
    swprintf_s(actionLabel, L"Set Action • %s", actionNames[g_selectedAction]);
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hActionSubmenu, actionLabel);
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
    AppendMenu(g_hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    HMENU hSettingsSubmenu = CreatePopupMenu();
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_fishstrapSupport.load() ? MF_CHECKED : 0), ID_FISHSTRAP_SUP, L"FishStrap/Shaders Support");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoUpdate.load() ? MF_CHECKED : 0), ID_AUTO_UPDATE, L"Update Checker [new]");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_userSafe.load() ? MF_CHECKED : 0), ID_USER_SAFE, L"User-Safe Mode [new] [wiki]");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSettingsSubmenu, L"Settings");
    AppendMenu(g_hMenu, MF_STRING | (g_multiSupport.load() ? MF_CHECKED : 0), ID_MULTI_SUPPORT, L"Multi-Instance Support");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    if (g_updateFound)
        AppendMenu(g_hMenu, MF_STRING, ID_UPDATE_AVAILABLE, L"[⭳] • Update Available");
    if (announcement_isEnabled && !announcementLabel.empty())
        AppendMenu(g_hMenu, MF_STRING, ID_ANNOUNCEMENT_TEXT, std::wstring(announcementLabel.begin(), announcementLabel.end()).c_str());
    if (announcement_isEnabled && !announcementLabel.empty())
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_EXIT, L"Exit");
}

void CheckForUpdates(bool showNotification = true)
{
    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/2.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return;
    HINTERNET hConnect = InternetOpenUrl(hInternet,
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/version",
        NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect)
        return;
    char buffer[16] = { 0 };
    DWORD bytesRead = 0;
    InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (bytesRead > 0)
    {
        buffer[bytesRead] = 0;
        int serverVersion = atoi(buffer);
        if (serverVersion > currentVersion)
        {
            wchar_t msg[128];
            swprintf_s(msg, L"New version available! Download on: https://github.com/Agzes/AntiAFK-RBX", currentVersion, serverVersion);
            ShowTrayNotification(L"AntiAFK-RBX | Update Available", msg);
            g_updateFound = true;
            CreateTrayMenu(g_isAfkStarted.load());
        }
    }
}

void CheckForAnnouncement()
{
    std::string configPath = "Software\\Agzes\\AntiAFK-RBX";
    int announcementID = 0;
    const wchar_t *urls[] = {
        L"https://raw.githubusercontent.com/Agzes/AHK-FOR-RPM/refs/heads/main/!Announcement/ID",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/isEnabled",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/isEveryRun",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/isNotify",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/label",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/link",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Announcement/text",};

    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/2.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return;

    for (int i = 0; i < 8; ++i)
    {
        HINTERNET hConnect = InternetOpenUrl(hInternet, urls[i], NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect)
            continue;

        char buffer[1024] = {0};
        DWORD bytesRead = 0;
        InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead);
        InternetCloseHandle(hConnect);

        if (bytesRead > 0)
        {
            buffer[bytesRead] = 0;
            switch (i) {
                case 0: { 
                    if (strcmp(buffer, "ERROR") != 0)
                        announcementID = atoi(buffer);
                    break;
                }
                case 1: {
                    if (strcmp(buffer, "ERROR") != 0)
                        announcement_isEnabled = (buffer[0] == '1');
                    break;
                }
                case 2: {
                    if (strcmp(buffer, "ERROR") != 0) {
                        bool isEveryRun = (buffer[0] == '1');
                        if (!isEveryRun) {
                            HKEY hKey;
                            DWORD currentID = 0;
                            DWORD dataSize = sizeof(DWORD);
                            if (RegOpenKeyExA(HKEY_CURRENT_USER, configPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                                RegQueryValueExA(hKey, "IDForAnnouncement", NULL, NULL, (LPBYTE)&currentID, &dataSize);
                                RegCloseKey(hKey);
                            }
                            if (announcementID <= currentID) {
                                InternetCloseHandle(hInternet);
                                return;
                            }
                        }
                    }
                    break;
                }
                case 3: {
                    if (strcmp(buffer, "ERROR") != 0)
                        announcement_isWithNotify = (buffer[0] == '1');
                    break;
                }
                case 4: {
                    if (strcmp(buffer, "ERROR") != 0)
                        announcementLabel = buffer;
                    break;
                }
                case 5: {
                    if (strcmp(buffer, "ERROR") != 0)
                        announcementLink = buffer;
                    break;
                }
                case 6: {
                    if (strcmp(buffer, "ERROR") != 0)
                        announcementText = buffer;
                    break;
                }
            }
        }
    }

    InternetCloseHandle(hInternet);

    if (announcement_isEnabled) {
        if (announcement_isWithNotify) {
            /* if (announcementLink == "None") {
                MessageBox(NULL, std::wstring(announcementText.begin(), announcementText.end()).c_str(), std::wstring(announcementLabel.begin(), announcementLabel.end()).c_str(), MB_OK);
            } else {
                int result = MessageBox(NULL, std::wstring(announcementText.begin(), announcementText.end()).c_str(), std::wstring(announcementLabel.begin(), announcementLabel.end()).c_str(), MB_YESNO);
                if (result == IDYES) {
                    ShellExecute(NULL, L"open", std::wstring(announcementLink.begin(), announcementLink.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
                }
            } */

            ShowTrayNotification(std::wstring(announcementLabel.begin(), announcementLabel.end()).c_str(), std::wstring(announcementText.begin(), announcementText.end()).c_str());                
        }

        HKEY hKey;
        if (RegCreateKeyExA(HKEY_CURRENT_USER, configPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            RegSetValueExA(hKey, "IDForAnnouncement", 0, REG_DWORD, (const BYTE *)&announcementID, sizeof(DWORD));
            RegCloseKey(hKey);
        }

        CreateTrayMenu(g_isAfkStarted.load());
    }
}

void MonitorUserActivity()
{
    g_monitorThreadRunning = true;
    g_lastActivityTime = GetTickCount64();
    while (g_monitorThreadRunning)
    {
        bool activity = false;

        for (int i = 1; i < 256 && !activity; i++)
        {
            if (GetAsyncKeyState(i) & 0x8000)
            {
                activity = true;
            }
        }

        if (!activity && (GetAsyncKeyState(VK_LBUTTON) & 0x8000 ||
                          GetAsyncKeyState(VK_RBUTTON) & 0x8000 ||
                          GetAsyncKeyState(VK_MBUTTON) & 0x8000))
        {
            activity = true;
        }

        if (activity)
        {
            g_lastActivityTime = GetTickCount64();
            g_userActive = true;
        }
        else
        {
            uint64_t currentTime = GetTickCount64();
            if ((currentTime - g_lastActivityTime) / 1000 >= USER_INACTIVITY_WAIT)
            {
                g_userActive = false;
            }
        }
        Sleep(100);
    }
}

void StartActivityMonitor()
{
    if (!g_monitorThreadRunning)
    {
        if (g_activityMonitorThread.joinable())
        {
            g_activityMonitorThread.join();
        }
        g_activityMonitorThread = std::thread(MonitorUserActivity);
    }
}

void StopActivityMonitor()
{
    g_monitorThreadRunning = false;
    if (g_activityMonitorThread.joinable())
    {
        g_activityMonitorThread.join();
    }
}

void main_thread()
{
    if (g_autoUpdate.load())
        CheckForUpdates();
    if (g_userSafe.load())
    {
        StartActivityMonitor();
    }
    CheckForAnnouncement();

    while (!g_stopThread.load())
    {
        std::unique_lock<std::mutex> lock(g_cv_m);
        if (g_isAfkStarted.load())
        {
            HWND user = GetForegroundWindow();
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                ShowTrayNotification(L"AntiAFK Error", L"Roblox window not found!");
            }
            else
            {
                if (g_userSafe.load())
                {
                    uint64_t startWaitTime = GetTickCount64();
                    bool notifiedUser = false;

                    while (true)
                    {
                        uint64_t currentTime = GetTickCount64();
                        uint64_t elapsedSeconds = (currentTime - startWaitTime) / 1000;

                        if (!g_userActive.load())
                        {
                            if (notifiedUser)
                            {
                                ShowTrayNotification(L"AntiAFK | User-Safe | Notice",
                                                     L"Performing anti-AFK action now.");
                            }
                            break;
                        }

                        if (elapsedSeconds >= MAX_WAIT_TIME)
                        {
                            ShowTrayNotification(L"AntiAFK | User-Safe | Notice",
                                                 L"Maximum wait time reached. Performing anti-AFK action now.");
                            break;
                        }

                        if (!notifiedUser && elapsedSeconds > 5)
                        {
                            ShowTrayNotification(L"AntiAFK | User-Safe | Waiting",
                                                 L"Waiting for keyboard/mouse inactivity before performing anti-AFK action...");
                            notifiedUser = true;
                        }

                        Sleep(500);

                        if (g_stopThread.load())
                            return;
                    }
                }

                for (int i = 0; i < 3; i++)  
                {
                    if (g_multiSupport.load())
                    {
                        for (HWND w : wins)
                        {
                            bool wasMinimized = IsIconic(w);
                            if (wasMinimized)
                            {
                                ShowWindow(w, SW_RESTORE);
                            }
                            SetForegroundWindow(w);
                            for (int i = 0; i < 3; i++)
                            {
                                AntiAFK_Action(w);
                            }
                            if (wasMinimized)
                            {
                                ShowWindow(w, SW_MINIMIZE);
                            }
                        }
                        break;
                    }
                    else
                    {
                        bool wasMinimized = IsIconic(wins.front());
                        if (wasMinimized)
                        {
                            ShowWindow(wins.front(), SW_RESTORE);
                        }
                        SetForegroundWindow(wins.front());
                        AntiAFK_Action(wins.front());
                        if (wasMinimized)
                        {
                            ShowWindow(wins.front(), SW_MINIMIZE);
                        }
                    }
                }
                RestoreForegroundWindow(user);
            }
            g_updateInterval = false;
            g_cv.wait_for(lock, std::chrono::seconds(g_selectedTime), []
                          { return g_stopThread.load() || g_updateInterval.load(); });
        }
        else
        {
            g_cv.wait_for(lock, 5000ms, []
                          { return g_stopThread.load(); });
        }
    }
    StopActivityMonitor();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        ApplyDarkMode(hwnd, true);
        g_nid.cbSize = sizeof(NOTIFYICONDATA);
        g_nid.hWnd = hwnd;
        g_nid.uID = ID_TRAY_ICON;
        g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_nid.uCallbackMessage = WM_USER + 1;
        g_nid.hIcon = CreateCustomIcon();
        lstrcpy(g_nid.szTip, L"v2.2 | AntiAFK-RBX");
        Shell_NotifyIcon(NIM_ADD, &g_nid);
        CreateTrayMenu(g_isAfkStarted.load());
        ShowWindow(hwnd, SW_HIDE);
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        break;
    case WM_SHOWWINDOW:
        if (wParam == TRUE)
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        break;
    case WM_USER + 1:
        if (lParam == WM_RBUTTONDOWN)
        {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            ApplyDarkMode(hwnd, true);
            TrackPopupMenu(g_hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_LINKTTU:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX/wiki/Tips-For-Use", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"Wiki > TipsToUse opened.");
            break;
        }
        case ID_LINKWIKI:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX/wiki", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"GitHub wiki opened.");
            break;
        }
        case ID_LINKGITHUB:
        case ID_UPDATE_AVAILABLE:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"GitHub link opened.");
            break;
        }
        case ID_LINKSF:
        {
            ShellExecute(NULL, L"open", L"https://antiafk-rbx.sourceforge.io", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"SourceForge link opened.");
            break;
        }
        case ID_INFORMATION:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"GitHub link opened.");
            break;
        }
        case ID_START_AFK:
        {
            auto wins = FindAllRobloxWindows(true);
            if (!wins.empty())
            {
                ShowTrayNotification(L"AntiAFK Started", L"AntiAFK has started.");
                g_isAfkStarted = true;
                CreateTrayMenu(true);
                UpdateTrayIcon();
            }
            else
            {
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
        case ID_SHOW_WINDOW:
        {
            if (!g_multiSupport.load())
            {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx)
                {
                    ShowWindow(rbx, SW_SHOW);
                }
                else if (g_fishstrapSupport.load())
                {
                    rbx = FindWindowByProcessName(L"eurotrucks2.exe");
                    if (rbx)
                    {
                        ShowWindow(rbx, SW_SHOW);
                    }
                    else
                    {
                        ShowTrayNotification(L"Error", L"Roblox window not found!");
                    }
                }
                else
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                }
            }
            else
            {
                ShowAllRobloxWindows_Multi();
            }
            break;
        }
        case ID_HIDE_WINDOW:
        {
            if (!g_multiSupport.load())
            {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx)
                {
                    ShowWindow(rbx, SW_HIDE);
                }
                else if (g_fishstrapSupport.load())
                {
                    rbx = FindWindowByProcessName(L"eurotrucks2.exe");
                    if (rbx)
                    {
                        ShowWindow(rbx, SW_HIDE);
                    }
                    else
                    {
                        ShowTrayNotification(L"Error", L"Roblox window not found!");
                    }
                }
                else
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                }
            }
            else
            {
                auto wins = FindAllRobloxWindows();
                for (HWND w : wins)
                    ShowWindow(w, SW_HIDE);
            }
            break;
        }
        case ID_EMULATE_SPACE:
        {
            auto wins = FindAllRobloxWindows(true);
            if (!wins.empty())
            {
                if (g_multiSupport.load())
                {
                    for (HWND w : wins)
                    {
                        bool wasMinimized = IsIconic(w);
                        if (wasMinimized)
                        {
                            ShowWindow(w, SW_RESTORE);
                        }
                        SetForegroundWindow(w);
                        AntiAFK_Action(w);
                        if (wasMinimized)
                        {
                            ShowWindow(w, SW_MINIMIZE);
                        }
                    }
                }
                else
                {
                    bool wasMinimized = IsIconic(wins.front());
                    if (wasMinimized)
                    {
                        ShowWindow(wins.front(), SW_RESTORE);
                    }
                    SetForegroundWindow(wins.front());
                    AntiAFK_Action(wins.front());
                    if (wasMinimized)
                    {
                        ShowWindow(wins.front(), SW_MINIMIZE);
                    }
                }
            }
            else
            {
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
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            UpdateTrayIcon();
            break;
        case ID_FISHSTRAP_SUP:
            g_fishstrapSupport = !g_fishstrapSupport.load();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            UpdateTrayIcon();
            break;
        case ID_AUTO_UPDATE:
            g_autoUpdate = !g_autoUpdate.load();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            if (g_autoUpdate.load())
                CheckForUpdates();
            break;
        case ID_USER_SAFE:
            g_userSafe = !g_userSafe.load();
            if (g_userSafe.load())
            {
                StartActivityMonitor();
                ShowTrayNotification(L"User-Safe Mode", L"User-Safe mode enabled. Check wiki for details.");
            }
            else
            {
                StopActivityMonitor();
            }
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_TIME_5_SEC:
        case ID_TIME_20_SEC:
        case ID_TIME_3:
        case ID_TIME_6:
        case ID_TIME_9:
        case ID_TIME_11:
        case ID_TIME_13:
        case ID_TIME_15:
        case ID_TIME_18:
            if (LOWORD(wParam) == ID_TIME_5_SEC)
                g_selectedTime = 5;
            else if (LOWORD(wParam) == ID_TIME_20_SEC)
                g_selectedTime = 20;
            else
            {
                g_selectedTime = (LOWORD(wParam) - ID_TIME_3 + 1) * 3;
                if (g_selectedTime == 12)
                    g_selectedTime = 11;
                if (g_selectedTime == 15)
                    g_selectedTime = 13;
                if (g_selectedTime == 21)
                    g_selectedTime = 18;
                g_selectedTime *= 60; 
            }
            g_updateInterval = true;
            g_cv.notify_all();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_ACTION_SPACE:
        case ID_ACTION_WS:
        case ID_ACTION_ZOOM:
            g_selectedAction = LOWORD(wParam) - ID_ACTION_SPACE;
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_ANNOUNCEMENT_TEXT:
            ShellExecute(NULL, L"open", std::wstring(announcementLink.begin(), announcementLink.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
            ShowTrayNotification(L"AntiAFK-RBX", L"Link opened.");
            break;
        case ID_EXIT:
            ShowAllRobloxWindows_Multi();
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    g_hInst = hInstance;
    const wchar_t CLASS_NAME[] = L"AntiAFK-RBX-tray";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    LoadSettings();
    RegisterClass(&wc);
    g_hwnd = CreateWindowEx(0, CLASS_NAME, L"AntiAFK-RBX-tray", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (g_multiSupport)
        EnableMultiInstanceSupport();
    ShowWindow(g_hwnd, SW_HIDE);
    std::thread t(main_thread);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    {
        std::lock_guard<std::mutex> lock(g_cv_m);
        g_stopThread = true;
        g_cv.notify_all();
    }
    if (t.joinable())
        t.join();
    DisableMultiInstanceSupport();
    StopActivityMonitor();
    return 0;
}