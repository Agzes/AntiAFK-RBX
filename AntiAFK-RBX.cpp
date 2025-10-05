// AntiAFK-RBX.cpp | The program for AntiAFK and Multi-Instance in Roblox. Or just Roblox Anti-AFK. | By Agzes
// https://github.com/Agzes/AntiAFK-RBX • \[=_=]/

int currentVersion = 8; // v.2.2.2

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <atomic>
#include <cstring>
#include <dwmapi.h>
#include <winreg.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")


using namespace std::chrono_literals;


#define ID_TRAY_ICON 0
#define ID_INFORMATION 1
#define ID_EMULATE_SPACE 2
#define ID_START_AFK 3
#define ID_STOP_AFK 4
#define ID_TIME_SUBMENU 100
#define ID_ACTION_SUBMENU 200
#define ID_SHOW_WINDOW 5
#define ID_HIDE_WINDOW 6
#define ID_SETTINGS_MENU 300
#define ID_MULTI_SUPPORT 7
#define ID_EXIT 8

#define ID_ABOUT_MENU 1011
#define ID_SHOW_TUTORIAL 1012
#define ID_LINKWIKI_TUTORIAL 1001
#define ID_LINKWIKI 1002
#define ID_LINKGITHUB 1003
#define ID_LINKSF 1004
#define ID_LINKTTU 1005

#define ID_TIME_CUSTOM 101
#define ID_TIME_3 103
#define ID_TIME_6 106
#define ID_TIME_9 109
#define ID_TIME_11 111
#define ID_TIME_13 113
#define ID_TIME_15 115
#define ID_TIME_18 118

#define ID_ACTION_SPACE 201
#define ID_ACTION_WS 202
#define ID_ACTION_ZOOM 203

#define ID_FISHSTRAP_SUP 301
#define ID_AUTO_UPDATE 302
#define ID_USER_SAFE 303
#define ID_AUTO_START_AFK 304
#define ID_RESET_SETTINGS 305

#define ID_UPDATE_AVAILABLE 1000
#define ID_ANNOUNCEMENT_TEXT 1001

#define ID_EDIT_SECONDS 1100
#define ID_BTN_GITHUB 1101
#define ID_BTN_CLOSE_ABOUT 1102
#define ID_BTN_NEXT 1201
#define ID_BTN_BACK 1202
#define ID_BTN_SKIP 1203


HWND g_hwnd;
HINSTANCE g_hInst;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
HANDLE g_hMultiInstanceMutex = NULL;
HWND g_hSplashWnd = NULL;

std::atomic<bool> g_isAfkStarted(false), g_stopThread(false), g_multiSupport(false), g_fishstrapSupport(false), g_autoUpdate(true), g_userSafe(false), g_updateFound(false), g_autoStartAfk(false), g_userActive(false), g_monitorThreadRunning(false), g_updateInterval(false), g_tutorialShown(false);
std::string announcementText, announcementLabel, announcementLink, announcementID;
std::atomic<uint64_t> g_lastActivityTime(0);
std::thread g_activityMonitorThread;
std::condition_variable g_cv;
std::mutex g_cv_m;

const TCHAR g_szClassName[] = _T("AntiAFK-RBX");
wchar_t g_splashStatus[128] = L"Initializing...";
constexpr DWORD ACTION_DELAY = 30, ALT_DELAY = 15;
int g_selectedTime = 540;
int g_selectedAction = 0; // 0 - space, 1 - w&s, 2 - zoom
const int USER_INACTIVITY_WAIT = 3;
const int MAX_WAIT_TIME = 60;
bool announcement_isWithNotify = false, announcement_isEnabled = false;
const COLORREF DARK_BG = RGB(30, 30, 30);
const COLORREF DARK_TEXT = RGB(235, 235, 235);
const COLORREF DARK_EDIT_BG = RGB(45, 45, 45);

struct EnumWindowsData
{
    DWORD processId;
    bool includeHidden;
    std::vector<HWND>* wins;
};

struct DarkMessageBoxParams
{
    const wchar_t* text;
    UINT type;
    int* result;
};

HICON CreateCustomIcon();
HICON CreateCustomIconWithState(bool isOn, bool multi);

void ApplyDarkMode(HWND hWnd, bool enable)
{
    HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll");
    if (hUxTheme)
    {
        typedef int(WINAPI* SetPreferredAppModeFunc)(int);
        typedef void(WINAPI* FlushMenuThemesFunc)(void);
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

void DrawThemedButton(LPDRAWITEMSTRUCT dis, bool isPrimary)
{
    COLORREF base = isPrimary ? RGB(0, 122, 204) : RGB(60, 60, 60);
    COLORREF basePressed = isPrimary ? RGB(0, 90, 160) : RGB(50, 50, 50);
    COLORREF border = isPrimary ? RGB(0, 140, 230) : RGB(90, 90, 90);

    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;
    COLORREF fill = pressed ? basePressed : base;
    if (disabled)
        fill = RGB(80, 80, 80);

    HPEN hPen = CreatePen(PS_SOLID, 1, border);
    HBRUSH hBrush = CreateSolidBrush(fill);
    HPEN oldPen = (HPEN)SelectObject(dis->hDC, hPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dis->hDC, hBrush);
    RoundRect(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom, 8, 8);
    SelectObject(dis->hDC, oldPen);
    SelectObject(dis->hDC, oldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    wchar_t text[64];
    GetWindowTextW(dis->hwndItem, text, 63);
    SetBkMode(dis->hDC, TRANSPARENT);
    SetTextColor(dis->hDC, disabled ? RGB(128, 128, 128) : RGB(255, 255, 255));
    DrawTextW(dis->hDC, text, -1, (LPRECT)&dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (dis->itemState & ODS_FOCUS)
    {
        RECT focusRect = dis->rcItem;
        InflateRect(&focusRect, -3, -3);
        DrawFocusRect(dis->hDC, &focusRect);
    }
}

LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFont28 = NULL, hFont12b = NULL, hFont12 = NULL, hFont10 = NULL;
    switch (msg)
    {
    case WM_CREATE:
    {
        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        hFont28 = CreateFontW(-MulDiv(28, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        hFont12b = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        hFont12 = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        hFont10 = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        return 0;
    }
    case WM_APP + 1:
        lstrcpynW(g_splashStatus, (const wchar_t*)wParam, 128);
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        HBRUSH bgBrush = CreateSolidBrush(DARK_BG);
        FillRect(hdc, &clientRect, bgBrush);
        DeleteObject(bgBrush);

        SetBkMode(hdc, TRANSPARENT);

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont28);

        const wchar_t* part1 = L"AntiAFK-";
        const wchar_t* part2 = L"RBX";
        SIZE size1, size2;
        GetTextExtentPoint32W(hdc, part1, lstrlenW(part1), &size1);
        GetTextExtentPoint32W(hdc, part2, lstrlenW(part2), &size2);

        int totalWidth = size1.cx + size2.cx;
        int x1 = (clientRect.right - totalWidth) / 2;
        int y_title = 55;

        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, x1, y_title, part1, lstrlenW(part1));
        SetTextColor(hdc, RGB(0xE2, 0x23, 0x1A));
        TextOutW(hdc, x1 + size1.cx, y_title, part2, lstrlenW(part2));

        SelectObject(hdc, hFont12b);
        SetTextColor(hdc, RGB(0x89, 0x89, 0x89));
        RECT by_rect = { 0, y_title + size1.cy - 10, clientRect.right, clientRect.bottom };
        DrawTextW(hdc, L"by Agzes", -1, &by_rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        SelectObject(hdc, hFont12);
        SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
        RECT note_rect = { 0, 170, clientRect.right, clientRect.bottom };
        DrawTextW(hdc, g_splashStatus, -1, &note_rect, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

        TEXTMETRIC tm13;
        GetTextMetrics(hdc, &tm13);
        SelectObject(hdc, hFont10);
        RECT version_rect = clientRect;
        version_rect.bottom = clientRect.bottom - 7 - 17;
        DrawTextW(hdc, L"v.2.2.2", -1, &version_rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

        SelectObject(hdc, hFont12);
        RECT loading_rect = clientRect;
        loading_rect.bottom = clientRect.bottom - 7;
        DrawTextW(hdc, L"LOADING", -1, &loading_rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

        SelectObject(hdc, hOldFont);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        if (hFont28)
        {
            DeleteObject(hFont28);
            hFont28 = NULL;
        }
        if (hFont12b)
        {
            DeleteObject(hFont12b);
            hFont12b = NULL;
        }
        if (hFont12)
        {
            DeleteObject(hFont12);
            hFont12 = NULL;
        }
        if (hFont10)
        {
            DeleteObject(hFont10);
            hFont10 = NULL;
        }
        g_hSplashWnd = NULL;
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void UpdateSplashStatus(const wchar_t *status)
{
    if (g_hSplashWnd)
        PostMessage(g_hSplashWnd, WM_APP + 1, (WPARAM)status, 0);
}

LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFontTitle = NULL, hFontText = NULL, hFontVersion = NULL, hHelpColor = NULL;
    static HBRUSH hbrBg = NULL;

    switch (msg)
    {
    case WM_CREATE:
    {
        ApplyDarkMode(hwnd, true);
        if (!hbrBg)
            hbrBg = CreateSolidBrush(DARK_BG);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        if (!hFontTitle)
            hFontTitle = CreateFontW(-MulDiv(20, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        if (!hHelpColor)
            hHelpColor = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        if (!hFontText)
            hFontText = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        if (!hFontVersion)
            hFontVersion = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        const int dlgW = 300, dlgH = 352;

        HWND hVersion = CreateWindowW(L"STATIC", L"v.2.2.2", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 6, dlgW, 20, hwnd, NULL, g_hInst, NULL);
        SendMessage(hVersion, WM_SETFONT, (WPARAM)hFontVersion, TRUE);

        HWND hIssue = CreateWindowW(L"STATIC", L"know bug/enchancement? write issue", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 250, 285, 20, hwnd, NULL, g_hInst, NULL);
        SendMessage(hIssue, WM_SETFONT, (WPARAM)hFontText, TRUE);

        const int btnW = 131, btnH = 32, gap = 8, margin = 20;
        int btnY = dlgH - margin - btnH - 26;
        int closeX = dlgW - margin - btnW - 3;
        int githubX = closeX - gap - btnW;

        HWND hGithub = CreateWindowW(L"BUTTON", L"GitHub", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, githubX, btnY, btnW, btnH, hwnd, (HMENU)ID_BTN_GITHUB, g_hInst, NULL);
        HWND hClose = CreateWindowW(L"BUTTON", L"Close", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW | BS_DEFPUSHBUTTON, closeX, btnY, btnW, btnH, hwnd, (HMENU)ID_BTN_CLOSE_ABOUT, g_hInst, NULL);
        SendMessage(hGithub, WM_SETFONT, (WPARAM)hFontText, TRUE);
        SendMessage(hClose, WM_SETFONT, (WPARAM)hFontText, TRUE);

        int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - dlgW) / 2, y = (screenH - dlgH) / 2;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, dlgW, dlgH, SWP_SHOWWINDOW);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, DARK_TEXT);
        return (LRESULT)hbrBg;
    }
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, hbrBg);
        return 1;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        SetBkMode(hdc, TRANSPARENT);

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFontTitle);

        const wchar_t* part1 = L"AntiAFK-";
        const wchar_t* part2 = L"RBX";
        SIZE size1, size2;
        GetTextExtentPoint32W(hdc, part1, lstrlenW(part1), &size1);
        GetTextExtentPoint32W(hdc, part2, lstrlenW(part2), &size2);

        int totalWidth = size1.cx + size2.cx;
        int x1 = (clientRect.right - totalWidth) / 2;
        int y_title = 50;

        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, x1, y_title, part1, lstrlenW(part1));
        SetTextColor(hdc, RGB(0xE2, 0x23, 0x1A));
        TextOutW(hdc, x1 + size1.cx, y_title, part2, lstrlenW(part2));

        SelectObject(hdc, hHelpColor);
        SetTextColor(hdc, RGB(0x89, 0x89, 0x89));
        RECT by_rect = { 0, y_title + size1.cy - 5, clientRect.right, clientRect.bottom };
        DrawTextW(hdc, L"by Agzes", -1, &by_rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        SelectObject(hdc, hOldFont);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == ID_BTN_GITHUB || dis->CtlID == ID_BTN_CLOSE_ABOUT)
        {
            DrawThemedButton(dis, dis->CtlID == ID_BTN_CLOSE_ABOUT);
            return TRUE;
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BTN_GITHUB:
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            break;
        case ID_BTN_CLOSE_ABOUT:
            DestroyWindow(hwnd);
            break;
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            SendMessage(hwnd, WM_COMMAND, ID_BTN_CLOSE_ABOUT, 0);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (hFontTitle)
        {
            DeleteObject(hFontTitle);
            hFontTitle = NULL;
        }
        if (hFontText)
        {
            DeleteObject(hFontText);
            hFontText = NULL;
        }
        if (hFontVersion)
        {
            DeleteObject(hFontVersion);
            hFontVersion = NULL;
        }
        if (hHelpColor)
        {
            DeleteObject(hHelpColor);
            hHelpColor = NULL;
        }
        if (hbrBg)
        {
            DeleteObject(hbrBg);
            hbrBg = NULL;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateSplashScreen(HINSTANCE hInstance)
{
    const wchar_t SPLASH_CLASS_NAME[] = L"AntiAFK-RBX-Splash";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = SplashWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = SPLASH_CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(DARK_BG);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = CreateCustomIcon();
    if (!RegisterClass(&wc))
        return;

    int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 286, winH = 312;
    int x = (screenW - winW) / 2, y = (screenH - winH) / 2;

    g_hSplashWnd = CreateWindowEx(WS_EX_TOPMOST, SPLASH_CLASS_NAME, L"AntiAFK-RBX", WS_POPUP | WS_CAPTION | WS_SYSMENU, x, y, winW, winH, NULL, NULL, hInstance, NULL);

    if (g_hSplashWnd)
    {
        ApplyDarkMode(g_hSplashWnd, true);
        ShowWindow(g_hSplashWnd, SW_SHOW);
        UpdateWindow(g_hSplashWnd);
    }
}

void ShowAboutDialog(HWND owner)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = AboutWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = L"AntiAFK-RBX-About";
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = CreateCustomIcon();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }
    HWND h = CreateWindowEx(WS_EX_TOPMOST, L"AntiAFK-RBX-About",
                            L"About AntiAFK-RBX", WS_POPUP | WS_CAPTION | WS_SYSMENU,
                            CW_USEDEFAULT, CW_USEDEFAULT, 300, 352, owner, NULL, g_hInst, NULL);
    if (h)
    {
        ShowWindow(h, SW_SHOW);
        UpdateWindow(h);
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

HICON CreateCustomIcon()
{
    return CreateCustomIconWithState(g_isAfkStarted.load(), g_multiSupport.load());
}

HICON CreateCustomIconWithState(bool isOn, bool multi)
{
    COLORREF innerColor = isOn ? (multi ? RGB(0xC8, 0x00, 0x64) : RGB(0x33, 0x5F, 0xFF)) : RGB(0x17, 0x17, 0x17);
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

void RestoreForegroundWindow(HWND prevWnd)
{
    if (!prevWnd)
        return;
    TCHAR className[256];
    GetClassName(prevWnd, className, 256);
    if (_tcscmp(className, L"AntiAFK-RBX-tray") == 0)
        return;
    if (!IsWindowVisible(prevWnd) || IsIconic(prevWnd))
        return;
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

void SaveSettings()
{
    HKEY hKey;
    DWORD multiSupport = g_multiSupport.load();
    DWORD fishstrapSupport = g_fishstrapSupport.load();
    DWORD autoUpdate = g_autoUpdate.load();
    DWORD userSafe = g_userSafe.load();
    DWORD autoStartAfk = g_autoStartAfk.load();
    DWORD tutorialShown = g_tutorialShown.load();

    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, L"MultiSupport", 0, REG_DWORD, (const BYTE *)&multiSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"FishstrapSupport", 0, REG_DWORD, (const BYTE *)&fishstrapSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedTime", 0, REG_DWORD, (const BYTE *)&g_selectedTime, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedAction", 0, REG_DWORD, (const BYTE *)&g_selectedAction, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoUpdate", 0, REG_DWORD, (const BYTE *)&autoUpdate, sizeof(DWORD));
        RegSetValueEx(hKey, L"UserSafe", 0, REG_DWORD, (const BYTE *)&userSafe, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoStartAfk", 0, REG_DWORD, (const BYTE *)&autoStartAfk, sizeof(DWORD));
        RegSetValueEx(hKey, L"TutorialShown", 0, REG_DWORD, (const BYTE *)&tutorialShown, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

void LoadSettings()
{
    HKEY hKey;
    DWORD multiSupport = 0, fishstrapSupport = 0, selectedTime = 540, selectedAction = 0, autoStartAfk = 0;
    DWORD autoUpdate = 1, userSafe = 0, autoLimitRam = 0;
    DWORD tutorialShown = 0;
    DWORD dataSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey, L"MultiSupport", NULL, NULL, (LPBYTE)&multiSupport, &dataSize);
        RegQueryValueEx(hKey, L"FishstrapSupport", NULL, NULL, (LPBYTE)&fishstrapSupport, &dataSize);
        RegQueryValueEx(hKey, L"SelectedTime", NULL, NULL, (LPBYTE)&selectedTime, &dataSize);
        RegQueryValueEx(hKey, L"SelectedAction", NULL, NULL, (LPBYTE)&selectedAction, &dataSize);
        RegQueryValueEx(hKey, L"AutoUpdate", NULL, NULL, (LPBYTE)&autoUpdate, &dataSize);
        RegQueryValueEx(hKey, L"UserSafe", NULL, NULL, (LPBYTE)&userSafe, &dataSize);
        RegQueryValueEx(hKey, L"AutoStartAfk", NULL, NULL, (LPBYTE)&autoStartAfk, &dataSize);
        RegQueryValueEx(hKey, L"TutorialShown", NULL, NULL, (LPBYTE)&tutorialShown, &dataSize);
        RegCloseKey(hKey);
    }

    g_multiSupport = multiSupport;
    g_fishstrapSupport = fishstrapSupport;
    g_selectedTime = selectedTime;
    g_selectedAction = selectedAction;
    g_autoUpdate = autoUpdate;
    g_userSafe = userSafe;
    g_autoStartAfk = autoStartAfk;
    g_tutorialShown = tutorialShown;
}

void ResetSettings()
{
    g_multiSupport = false;
    g_fishstrapSupport = false;
    g_selectedTime = 540;
    g_selectedAction = 0;
    g_autoUpdate = true;
    g_userSafe = false;
    g_autoStartAfk = false;
    g_tutorialShown = false;

    if (g_monitorThreadRunning.load())
    {
        StopActivityMonitor();
    }

    if (g_hMultiInstanceMutex)
    {
        DisableMultiInstanceSupport();
    }

    RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX");
}

void CreateTrayMenu(bool afk)
{
    if (g_hMenu)
        DestroyMenu(g_hMenu);
    g_hMenu = CreatePopupMenu();

    HMENU InfoMenu = CreatePopupMenu();
    AppendMenu(InfoMenu, MF_STRING, ID_ABOUT_MENU, L"About AntiAFK-RBX");
    AppendMenu(InfoMenu, MF_STRING, ID_SHOW_TUTORIAL, L"Show Tutorial");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKTTU, L"[Link] - TipsToUse");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKWIKI, L"[Link] - Wiki");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKGITHUB, L"[Link] - GitHub");
    AppendMenu(InfoMenu, MF_STRING, ID_LINKSF, L"[Link] - SourceForge");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)InfoMenu, L"AntiAFK-RBX by Agzes");

    AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, L"Beta: v.2.2.2 | With ❤️");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_EMULATE_SPACE, L"Test Anti-AFK move");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);

    HMENU hTimeSubmenu = CreatePopupMenu();
    AppendMenu(hTimeSubmenu, MF_STRING, ID_TIME_CUSTOM, L"⏱️ Custom interval >");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 3 * 60 ? MF_CHECKED : 0), ID_TIME_3, L"3 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 6 * 60 ? MF_CHECKED : 0), ID_TIME_6, L"6 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 9 * 60 ? MF_CHECKED : 0), ID_TIME_9, L"9 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 11 * 60 ? MF_CHECKED : 0), ID_TIME_11, L"11 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 13 * 60 ? MF_CHECKED : 0), ID_TIME_13, L"13 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 15 * 60 ? MF_CHECKED : 0), ID_TIME_15, L"15 minutes");
    AppendMenu(hTimeSubmenu, MF_STRING | (g_selectedTime == 18 * 60 ? MF_CHECKED : 0), ID_TIME_18, L"18 minutes");
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
    const wchar_t *actionNames[] = {L"Space (Jump)", L"W/S", L"Zoom (I/O)"};
    swprintf_s(actionLabel, L"Set Action • %s", actionNames[g_selectedAction]);
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hActionSubmenu, actionLabel);
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
    AppendMenu(g_hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    HMENU hSettingsSubmenu = CreatePopupMenu();
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_fishstrapSupport.load() ? MF_CHECKED : 0), ID_FISHSTRAP_SUP, L"FishStrap/Shaders Support");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoUpdate.load() ? MF_CHECKED : 0), ID_AUTO_UPDATE, L"Update Checker");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_userSafe.load() ? MF_CHECKED : 0), ID_USER_SAFE, L"User-Safe Mode [see wiki]");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoStartAfk.load() ? MF_CHECKED : 0), ID_AUTO_START_AFK, L"Auto-Start AntiAFK [new]");
    AppendMenu(hSettingsSubmenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSettingsSubmenu, MF_STRING, ID_RESET_SETTINGS, L"Reset Settings");
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

void UpdateTrayIcon()
{
    g_nid.hIcon = CreateCustomIcon();
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}

void CheckForUpdates(bool showNotification = true)
{
    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/2.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return;
    HINTERNET hConnect = InternetOpenUrl(hInternet, L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/version", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect)
        return;
    char buffer[16] = {0};
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
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/ID",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/isEnabled",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/isEveryRun",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/isNotify",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/label",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/link",
        L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/main/Announcement/text",
    };

    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/2.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return;

    for (int i = 0; i < 7; ++i)
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
            switch (i)
            {
            case 0:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcementID = atoi(buffer);
                break;
            }
            case 1:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcement_isEnabled = (buffer[0] == '1');
                break;
            }
            case 2:
            {
                if (strcmp(buffer, "ERROR") != 0)
                {
                    bool isEveryRun = (buffer[0] == '1');
                    if (!isEveryRun)
                    {
                        HKEY hKey;
                        DWORD currentID = 0;
                        DWORD dataSize = sizeof(DWORD);
                        if (RegOpenKeyExA(HKEY_CURRENT_USER, configPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
                        {
                            RegQueryValueExA(hKey, "IDForAnnouncement", NULL, NULL, (LPBYTE)&currentID, &dataSize);
                            RegCloseKey(hKey);
                        }
                        if (announcementID <= currentID)
                        {
                            InternetCloseHandle(hInternet);
                            return;
                        }
                    }
                }
                break;
            }
            case 3:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcement_isWithNotify = (buffer[0] == '1');
                break;
            }
            case 4:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcementLabel = buffer;
                break;
            }
            case 5:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcementLink = buffer;
                break;
            }
            case 6:
            {
                if (strcmp(buffer, "ERROR") != 0)
                    announcementText = buffer;
                break;
            }
            }
        }
    }

    InternetCloseHandle(hInternet);

    if (announcement_isEnabled)
    {
        if (announcement_isWithNotify)
        {
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
        if (RegCreateKeyExA(HKEY_CURRENT_USER, configPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
        {
            RegSetValueExA(hKey, "IDForAnnouncement", 0, REG_DWORD, (const BYTE *)&announcementID, sizeof(DWORD));
            RegCloseKey(hKey);
        }

        CreateTrayMenu(g_isAfkStarted.load());
    }
}

LRESULT CALLBACK CustomIntervalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFont = NULL;
    static HBRUSH hbrBg = NULL;
    static HBRUSH hbrEditBg = NULL;
    switch (msg)
    {
    case WM_CREATE:
    {
        ApplyDarkMode(hwnd, true);
        if (!hbrBg)
            hbrBg = CreateSolidBrush(DARK_BG);
        if (!hbrEditBg)
            hbrEditBg = CreateSolidBrush(DARK_EDIT_BG);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);
        int titleH = -MulDiv(12, dpiY, 72);
        int textH = -MulDiv(10, dpiY, 72);
        if (!hFont)
            hFont = CreateFontW(textH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        const int margin = 20;
        const int dlgW = 300;
        const int dlgH = 135;

        HWND hLbl = CreateWindowW(L"STATIC", L"Enter interval in seconds:", WS_CHILD | WS_VISIBLE,
                                  7, 7, 220, 20, hwnd, NULL, g_hInst, NULL);
        SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL | WS_TABSTOP,
                                   7, margin + 7, 270, 23, hwnd, (HMENU)ID_EDIT_SECONDS, g_hInst, NULL);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        const int btnW = 131, btnH = 32, gap = 8;
        int btnY = dlgH - margin - btnH - 26;
        int cancelX = dlgW - margin - btnW - 3;
        int okX = cancelX - gap - btnW;

        HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW | BS_DEFPUSHBUTTON, okX, btnY, btnW, btnH, hwnd, (HMENU)IDOK, g_hInst, NULL);
        HWND hCancel = CreateWindowW(L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, cancelX, btnY, btnW, btnH, hwnd, (HMENU)IDCANCEL, g_hInst, NULL);
        SendMessage(hOk, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);

        wchar_t buf[32];
        _itow_s(g_selectedTime, buf, 10);
        SetWindowTextW(hEdit, buf);
        SendMessage(hEdit, EM_SETLIMITTEXT, 6, 0);
        SetFocus(hEdit);

        RECT wa;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
        POINT pt;
        GetCursorPos(&pt);
        int x = pt.x - dlgW / 2;
        int y = pt.y - dlgH / 2;
        if (x < wa.left)
            x = wa.left;
        if (y < wa.top)
            y = wa.top;
        if (x + dlgW > wa.right)
            x = wa.right - dlgW;
        if (y + dlgH > wa.bottom)
            y = wa.bottom - dlgH;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, dlgW, dlgH, SWP_SHOWWINDOW);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, OPAQUE);
        SetBkColor(hdc, (msg == WM_CTLCOLOREDIT) ? DARK_EDIT_BG : DARK_BG);
        SetTextColor(hdc, DARK_TEXT);
        return (LRESULT)((msg == WM_CTLCOLOREDIT) ? hbrEditBg : hbrBg);
    }
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH br = CreateSolidBrush(DARK_BG);
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        return 1;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == IDOK || dis->CtlID == IDCANCEL)
        {
            DrawThemedButton(dis, dis->CtlID == IDOK);
            return TRUE;
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            wchar_t buf[64] = {0};
            GetWindowTextW(GetDlgItem(hwnd, ID_EDIT_SECONDS), buf, 63);
            int sec = _wtoi(buf);
            if (sec <= 0)
                sec = 1;
            if (sec > 1200)
                sec = 1200; // limit to 20m
            g_selectedTime = sec;
            g_updateInterval = true;
            g_cv.notify_all();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            DestroyWindow(hwnd);
            break;
        }
        case IDCANCEL:
            DestroyWindow(hwnd);
            break;
        }
        break;
    }
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            SendMessage(hwnd, WM_COMMAND, IDOK, 0);
            return 0;
        }
        else if (wParam == VK_ESCAPE)
        {
            SendMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (hFont)
        {
            DeleteObject(hFont);
            hFont = NULL;
        }
        if (hbrBg)
        {
            DeleteObject(hbrBg);
            hbrBg = NULL;
        }
        if (hbrEditBg)
        {
            DeleteObject(hbrEditBg);
            hbrEditBg = NULL;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowCustomIntervalDialog(HWND owner)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = CustomIntervalWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = L"AntiAFK-RBX-CustomInterval";
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = CreateCustomIcon();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }
    HWND h = CreateWindowEx(WS_EX_TOPMOST, L"AntiAFK-RBX-CustomInterval",
                            L"AntiAFK-RBX | Set AntiAFK Interval", WS_POPUP | WS_CAPTION | WS_SYSMENU,
                            CW_USEDEFAULT, CW_USEDEFAULT, 300, 135, owner, NULL, g_hInst, NULL);
    if (h)
    {
        ShowWindow(h, SW_SHOW);
        UpdateWindow(h);
    }
}

LRESULT CALLBACK TutorialWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFont40 = NULL, hFont20 = NULL, hFont14 = NULL, hFont13 = NULL, hFont11 = NULL, hFont14b = NULL;
    static HBRUSH hbrBg = NULL;
    static HWND hBtnNext = NULL, hBtnBack = NULL, hBtnNo = NULL, hLblSkip = NULL, hLinkGitHub = NULL, hLinkWiki = NULL;
    static int page = 0; // 0: welcome, 1: htu, 2: htw, 3: tips, 4: usersafe, 5: final

    switch (msg)
    {
    case WM_CREATE:
    {
        ApplyDarkMode(hwnd, true);
        if (!hbrBg)
            hbrBg = CreateSolidBrush(DARK_BG);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        if (!hFont40)
            hFont40 = CreateFontW(-MulDiv(30, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        if (!hFont20)
            hFont20 = CreateFontW(-MulDiv(16, dpiY, 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        if (!hFont14)
            hFont14 = CreateFontW(-MulDiv(14, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        if (!hFont14b)
            hFont14b = CreateFontW(-MulDiv(14, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        if (!hFont13)
            hFont13 = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        if (!hFont11)
            hFont11 = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        const int dlgW = 472, dlgH = 312;
        const int margin = 16;
        const int btnW = 131, btnH = 32;

        hBtnNext = CreateWindowW(L"BUTTON", L"Next", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW | BS_DEFPUSHBUTTON,
                                 dlgW - margin - btnW - 10, dlgH - margin - btnH - 33, btnW, btnH, hwnd, (HMENU)ID_BTN_NEXT, g_hInst, NULL);
        hBtnBack = CreateWindowW(L"BUTTON", L"Back", WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
                                 margin - 6, dlgH - margin - btnH - 33, btnW, btnH, hwnd, (HMENU)ID_BTN_BACK, g_hInst, NULL);
        hBtnNo = CreateWindowW(L"BUTTON", L"No", WS_CHILD | WS_TABSTOP | BS_OWNERDRAW, 0, 0, btnW, btnH, hwnd, (HMENU)IDNO, g_hInst, NULL);
        SendMessage(hBtnNext, WM_SETFONT, (WPARAM)hFont13, TRUE);
        SendMessage(hBtnBack, WM_SETFONT, (WPARAM)hFont13, TRUE);
        SendMessage(hBtnNo, WM_SETFONT, (WPARAM)hFont13, TRUE);

        hLblSkip = CreateWindowW(L"STATIC", L"skip", WS_CHILD | WS_VISIBLE | SS_NOTIFY,
                                 dlgW - margin - btnW - 65, dlgH - margin - btnH - 35 + 7, 48, 24, hwnd, (HMENU)ID_BTN_SKIP, g_hInst, NULL);
        SendMessage(hLblSkip, WM_SETFONT, (WPARAM)hFont13, TRUE);

        hLinkGitHub = CreateWindowW(L"STATIC", L"More info in AntiAFK-RBX GitHub", WS_CHILD | SS_NOTIFY,
                                    10, 150, 350, 20, hwnd, (HMENU)ID_LINKGITHUB, g_hInst, NULL);
        SendMessage(hLinkGitHub, WM_SETFONT, (WPARAM)hFont13, TRUE);

        hLinkWiki = CreateWindowW(L"STATIC", L"More info in AntiAFK-RBX Wiki", WS_CHILD | SS_NOTIFY,
                                  10, 150, 350, 20, hwnd, (HMENU)ID_LINKWIKI_TUTORIAL, g_hInst, NULL);
        SendMessage(hLinkWiki, WM_SETFONT, (WPARAM)hFont13, TRUE);

        int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - dlgW) / 2, y = (screenH - dlgH) / 2;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, dlgW, dlgH, SWP_SHOWWINDOW);

        ShowWindow(hLinkGitHub, SW_HIDE);
        ShowWindow(hLinkWiki, SW_HIDE);
        ShowWindow(hBtnBack, SW_HIDE);
        ShowWindow(hBtnNo, SW_HIDE);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        if (msg == WM_CTLCOLORSTATIC && ((HWND)lParam == hLblSkip || (HWND)lParam == hLinkGitHub || (HWND)lParam == hLinkWiki))
            SetTextColor(hdc, RGB(0x77, 0x77, 0x77));
        else
            SetTextColor(hdc, DARK_TEXT);
        return (LRESULT)hbrBg;
    }
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, hbrBg);
        return 1;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == ID_BTN_NEXT || dis->CtlID == ID_BTN_BACK || dis->CtlID == IDNO)
        {
            DrawThemedButton(dis, dis->CtlID == ID_BTN_NEXT);
            return TRUE;
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_LINKGITHUB:
            if (HIWORD(wParam) == STN_CLICKED)
            {
                ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            }
            break;
        case ID_LINKWIKI_TUTORIAL:
            if (HIWORD(wParam) == STN_CLICKED)
            {
                ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX/wiki/User-Safe-Mode", NULL, NULL, SW_SHOWNORMAL);
            }
            break;
        case ID_BTN_SKIP:
            if (HIWORD(wParam) == STN_CLICKED || HIWORD(wParam) == 0)
            {
                g_tutorialShown = true;
                SaveSettings();
                DestroyWindow(hwnd);
            }
            break;
        case ID_BTN_NEXT:
            if (page == 5)
            {
                g_tutorialShown = true;
                SaveSettings();
                CreateTrayMenu(g_isAfkStarted.load());
                DestroyWindow(hwnd);
            }
            else
            {
                if (page == 3)
                {
                    g_autoStartAfk = true;
                }
                if (page == 4)
                {
                    g_userSafe = true;
                    if (!g_monitorThreadRunning)
                        StartActivityMonitor();
                }
                page++;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case ID_BTN_BACK:
            if (page > 0)
            {
                page--;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case IDNO:
            if (page == 3 || page == 4)
            {
                if (page == 3)
                {
                    g_autoStartAfk = false;
                }
                if (page == 4)
                {
                    g_userSafe = false;
                    if (g_monitorThreadRunning)
                        StopActivityMonitor();
                }
                page++;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        }
        break;
    }
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        HBRUSH bg = CreateSolidBrush(DARK_BG);
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);
        SetBkMode(hdc, TRANSPARENT);
        const int dlgW = 472, dlgH = 312;

        ShowWindow(hLblSkip, page == 0 ? SW_SHOW : SW_HIDE);
        ShowWindow(hBtnBack, page > 0 ? SW_SHOW : SW_HIDE);
        ShowWindow(hLinkGitHub, page == 2 ? SW_SHOW : SW_HIDE);
        ShowWindow(hLinkWiki, page == 4 ? SW_SHOW : SW_HIDE);
        ShowWindow(hBtnNo, (page == 3 || page == 4) ? SW_SHOW : SW_HIDE);

        if (page == 3 || page == 4)
        {
            SetWindowTextW(hBtnNext, L"Yes");
            const int margin = 16, btnW = 131, btnH = 32, gap = 10;
            int y_pos = dlgH - margin - btnH - 33;
            int x_yes = dlgW - margin - btnW - 10;
            int x_no = x_yes - btnW - gap;
            SetWindowPos(hBtnNo, NULL, x_no, y_pos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        else if (page == 5)
        {
            SetWindowTextW(hBtnNext, L"Finish");
        }
        else
        {
            SetWindowTextW(hBtnNext, L"Next");
        }

        if (page == 0)
        {
            HFONT old = (HFONT)SelectObject(hdc, hFont11);
            SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
            RECT rTop = {rc.left + 8, rc.top + 8, rc.right - 10, rc.top + 40};
            DrawTextW(hdc, L"MIT License", -1, &rTop, DT_TOP | DT_RIGHT | DT_SINGLELINE);

            int left = 10;
            int centerY = (rc.bottom - rc.top) / 2;
            int y = centerY - 75;

            SelectObject(hdc, hFont20);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, left + 4, y, L"Welcome to", 10);

            y += 16;
            SelectObject(hdc, hFont40);
            const wchar_t *p1 = L"AntiAFK-";
            const wchar_t *p2 = L"RBX";
            SIZE s1, s2;
            GetTextExtentPoint32W(hdc, p1, lstrlenW(p1), &s1);
            GetTextExtentPoint32W(hdc, p2, lstrlenW(p2), &s2);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, left, y, p1, lstrlenW(p1));
            SetTextColor(hdc, RGB(0xE2, 0x23, 0x1A));
            TextOutW(hdc, left + s1.cx, y, p2, lstrlenW(p2));

            y += s2.cy - 8;
            SelectObject(hdc, hFont11);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, left, y, L"The program for AntiAFK and Multi-Instance in Roblox", 54);

            SelectObject(hdc, old);
        }
        else if (page == 1)
        {
            const int margin = 10;
            HFONT old = (HFONT)SelectObject(hdc, hFont14b);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, margin, margin, L"How to use?", 12);

            SelectObject(hdc, hFont13);
            TextOutW(hdc, dlgW - 56, margin, L"1/5", 3);

            SelectObject(hdc, hFont13);
            RECT rText = {margin, margin + 34, rc.right - margin, rc.top + 150};
            const wchar_t *howto = L"AntiAFK-RBX its a tray program, all settings (interval, action, function and etc.) changing in tray menu (left to clock) shown when RightClick on AntiAFK-RBX dynamic tray-logo";
            DrawTextW(hdc, howto, -1, &rText, DT_WORDBREAK);

            RECT rPrevLabel = {margin, rText.bottom - 35, rc.right - margin, rText.bottom - 14};
            DrawTextW(hdc, L"dynamic tray-logo preview:", -1, &rPrevLabel, DT_CENTER | DT_TOP | DT_SINGLELINE);

            int iconsY = rPrevLabel.bottom + 8;
            int iconSize = 32;
            int gap = 100;
            int totalW = iconSize * 3 + gap * 2;
            int startX = (rc.right - rc.left - totalW) / 2;
            HICON icoOff = CreateCustomIconWithState(false, false);
            HICON icoOnSingle = CreateCustomIconWithState(true, false);
            HICON icoOnMulti = CreateCustomIconWithState(true, true);
            DrawIconEx(hdc, startX + 0 * (iconSize + gap), iconsY, icoOnSingle, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DrawIconEx(hdc, startX + 1 * (iconSize + gap), iconsY, icoOff, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DrawIconEx(hdc, startX + 2 * (iconSize + gap), iconsY, icoOnMulti, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DestroyIcon(icoOnSingle);
            DestroyIcon(icoOff);
            DestroyIcon(icoOnMulti);

            SelectObject(hdc, hFont11);
            RECT rL1 = {startX - 70, iconsY + iconSize + 4, startX + iconSize + 80, iconsY + iconSize + 28};
            RECT rL2 = {startX + (iconSize + gap) - 223, iconsY + iconSize + 4, startX + (iconSize + gap) + iconSize + 220, iconsY + iconSize + 28};
            RECT rL3 = {startX + 2 * (iconSize + gap) - 53, iconsY + iconSize + 4, startX + 2 * (iconSize + gap) + iconSize + 240, iconsY + iconSize + 28};
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, L"ON - Single instance", -1, &rL1, DT_CENTER | DT_TOP | DT_SINGLELINE);
            DrawTextW(hdc, L"OFF", -1, &rL2, DT_CENTER | DT_TOP | DT_SINGLELINE);
            DrawTextW(hdc, L"ON - Multi instance", -1, &rL3, DT_LEFT | DT_TOP | DT_SINGLELINE);

            SelectObject(hdc, hFont13);
            SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
            RECT rHint = {margin, rc.bottom - 26 - 32 - 12, rc.right - margin, rc.bottom};
            DrawTextW(hdc, L"tray logo can be hided in Windows tray folder (“ ^ ”)", -1, &rHint, DT_LEFT | DT_TOP | DT_SINGLELINE);

            SelectObject(hdc, old);
        }
        else if (page == 2)
        {
            const int margin = 10;
            HFONT old = (HFONT)SelectObject(hdc, hFont14b);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, margin, margin, L"How it work?", 12);

            SelectObject(hdc, hFont13);
            TextOutW(hdc, dlgW - 56, margin, L"2/5", 3);

            RECT rText = {margin, margin + 34, rc.right - margin, rc.top + 150};
            const wchar_t *workText = L"When the program starts, it starts another thread (AntiAFK thread), which checks whether the roblox window is running, and then performs actions on the user's settings, if the settings change during the process, the thread is restarted.";
            DrawTextW(hdc, workText, -1, &rText, DT_WORDBREAK);

            RECT rLink;
            GetClientRect(hLinkGitHub, &rLink);
            SetWindowPos(hLinkGitHub, NULL, margin, 155, rLink.right - rLink.left, rLink.bottom - rLink.top, SWP_NOZORDER | SWP_NOSIZE);

            SelectObject(hdc, old);
        }
        else if (page == 3)
        {
            const int margin = 10;
            HFONT old = (HFONT)SelectObject(hdc, hFont14b);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, margin, margin, L"Tips To Use", 11);

            SelectObject(hdc, hFont13);
            TextOutW(hdc, dlgW - 56, margin, L"3/5", 3);

            RECT rText = {margin + 4, margin + 50, rc.right - margin - 4, rc.bottom};
            const wchar_t *tipsText = L"“AutoStart” Function - auto start/stop antiafk action if roblox is open/close, work only when AntiAFK-RBX is open. Recommended to turn it on. Turn it on?";
            RECT rTextActual = rText;
            DrawTextW(hdc, tipsText, -1, &rTextActual, DT_WORDBREAK | DT_CALCRECT);
            DrawTextW(hdc, tipsText, -1, &rTextActual, DT_WORDBREAK);

            SelectObject(hdc, hFont11);
            SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
            RECT rSubText = {margin, rTextActual.bottom + 8, rc.right - margin, rc.bottom};
            DrawTextW(hdc, L"This can be changed in tray (Settings > AutoStart)", -1, &rSubText, DT_LEFT | DT_WORDBREAK);

            SelectObject(hdc, old);
        }
        else if (page == 4)
        {
            const int margin = 10;
            HFONT old = (HFONT)SelectObject(hdc, hFont14b);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, margin, margin, L"Tips To Use                          ", 26);

            SelectObject(hdc, hFont13);
            TextOutW(hdc, dlgW - 56, margin, L"4/5", 3);

            RECT rText = {margin + 4, margin + 50, rc.right - margin - 4, rc.bottom};
            const wchar_t *tipsText = L"“User-Safe Mode” Function allows you to conveniently use your PC while AntiAFK is enabled. Recommended to turn it on. Turn it on?";
            RECT rTextActual = rText;
            DrawTextW(hdc, tipsText, -1, &rTextActual, DT_WORDBREAK | DT_CALCRECT);
            DrawTextW(hdc, tipsText, -1, &rTextActual, DT_WORDBREAK);

            RECT rLink;
            GetClientRect(hLinkWiki, &rLink);
            SetWindowPos(hLinkWiki, NULL, margin, rTextActual.bottom + 8, rLink.right - rLink.left, rLink.bottom - rLink.top, SWP_NOZORDER | SWP_NOSIZE);

            SelectObject(hdc, old);
        }
        else if (page == 5)
        {
            HFONT old = (HFONT)SelectObject(hdc, hFont11);
            SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
            RECT rTop = {rc.left + 8, rc.top + 8, rc.right - 10, rc.top + 40};
            DrawTextW(hdc, L"Finish", -1, &rTop, DT_TOP | DT_RIGHT | DT_SINGLELINE);

            int centerY = (rc.bottom - rc.top) / 2;
            int y = centerY - 75;

            SelectObject(hdc, hFont20);
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT rThanks = {0, y, rc.right, y + 30};
            DrawTextW(hdc, L"Thank for using", -1, &rThanks, DT_CENTER | DT_TOP | DT_SINGLELINE);

            y += 26;
            SelectObject(hdc, hFont40);
            const wchar_t *p1 = L"AntiAFK-";
            const wchar_t *p2 = L"RBX";
            SIZE s1, s2;
            GetTextExtentPoint32W(hdc, p1, lstrlenW(p1), &s1);
            GetTextExtentPoint32W(hdc, p2, lstrlenW(p2), &s2);
            int totalW = s1.cx + s2.cx;
            int startX = (rc.right - totalW) / 2;
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, startX, y, p1, lstrlenW(p1));
            SetTextColor(hdc, RGB(0xE2, 0x23, 0x1A));
            TextOutW(hdc, startX + s1.cx, y, p2, lstrlenW(p2));

            y += s2.cy + 20;
            SelectObject(hdc, hFont11);
            SetTextColor(hdc, RGB(0x7B, 0x7B, 0x7B));
            RECT rBottom = {0, y, rc.right, rc.bottom - 30};
            DrawTextW(hdc, L"Tutorial can be open again in tray (AntiAFK-RBX > Show Tutorial)", -1, &rBottom, DT_CENTER | DT_WORDBREAK);

            SelectObject(hdc, old);
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (hFont40)
        {
            DeleteObject(hFont40);
            hFont40 = NULL;
        }
        if (hFont20)
        {
            DeleteObject(hFont20);
            hFont20 = NULL;
        }
        if (hFont14)
        {
            DeleteObject(hFont14);
            hFont14 = NULL;
        }
        if (hFont14b)
        {
            DeleteObject(hFont14b);
            hFont14b = NULL;
        }
        if (hFont13)
        {
            DeleteObject(hFont13);
            hFont13 = NULL;
        }
        if (hFont11)
        {
            DeleteObject(hFont11);
            hFont11 = NULL;
        }
        if (hbrBg)
        {
            DeleteObject(hbrBg);
            hbrBg = NULL;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowTutorialDialog(HWND owner)
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = TutorialWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = L"AntiAFK-RBX-Tutorial";
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = CreateCustomIcon();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }
    HWND h = CreateWindowEx(WS_EX_TOPMOST, L"AntiAFK-RBX-Tutorial", L"AntiAFK-RBX | Tutorial", WS_POPUP | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 472, 312, owner, NULL, g_hInst, NULL);
    if (h)
    {
        ShowWindow(h, SW_SHOW);
        UpdateWindow(h);
    }
}

LRESULT CALLBACK DarkMessageBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFontText = NULL;
    static HBRUSH hbrBg = NULL;
    static HBRUSH hbrButtonPaneBg = NULL;
    DarkMessageBoxParams *params = (DarkMessageBoxParams *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        ApplyDarkMode(hwnd, true);
        if (!hbrBg)
            hbrBg = CreateSolidBrush(DARK_BG);
        if (!hbrButtonPaneBg)
            hbrButtonPaneBg = CreateSolidBrush(DARK_EDIT_BG);

        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        params = (DarkMessageBoxParams *)lpcs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)params);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        if (!hFontText)
            hFontText = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        HDC hdc = CreateCompatibleDC(screen);
        ReleaseDC(NULL, screen);
        HFONT oldFont = (HFONT)SelectObject(hdc, hFontText);

        const int main_margin = 20;
        const int button_margin = 10;
        const int icon_size = 32;
        const int icon_text_gap = 15;
        int dlgW = 380;

        RECT textRect = {0, 0, dlgW - main_margin * 2 - icon_size - icon_text_gap, 0};
        DrawTextW(hdc, params->text, -1, &textRect, DT_CALCRECT | DT_WORDBREAK);
        SelectObject(hdc, oldFont);
        DeleteDC(hdc);

        int textH = textRect.bottom;
        int contentH = max(textH, icon_size);

        const int btnH = 32;
        const int button_pane_height = btnH + 2 * button_margin;
        int dlgH = main_margin + contentH + main_margin + button_pane_height;

        UINT iconType = params->type & 0xF0;
        LPCWSTR iconId = NULL;
        if (iconType == MB_ICONINFORMATION)
            iconId = IDI_INFORMATION;
        else if (iconType == MB_ICONWARNING)
            iconId = IDI_WARNING;
        else if (iconType == MB_ICONERROR)
            iconId = IDI_ERROR;
        else if (iconType == MB_ICONQUESTION)
            iconId = IDI_QUESTION;

        if (iconId)
        {
            HICON hIcon = LoadIcon(NULL, iconId);
            int iconY = main_margin + (contentH - icon_size) / 2;
            HWND hIconCtl = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ICON, main_margin, iconY, icon_size, icon_size, hwnd, NULL, g_hInst, NULL);
            SendMessage(hIconCtl, STM_SETICON, (WPARAM)hIcon, 0);
        }

        int textY = main_margin + (contentH - textH) / 2;
        HWND hText = CreateWindowW(L"STATIC", params->text, WS_CHILD | WS_VISIBLE | SS_LEFT, main_margin + icon_size + icon_text_gap, textY, textRect.right, textH, hwnd, NULL, g_hInst, NULL);
        SendMessage(hText, WM_SETFONT, (WPARAM)hFontText, TRUE);

        UINT btnType = params->type & 0x0F;
        std::vector<std::pair<int, const wchar_t *>> buttons;
        if (btnType == MB_OK)
        {
            buttons.push_back({IDOK, L"OK"});
        }
        else if (btnType == MB_OKCANCEL)
        {
            buttons.push_back({IDOK, L"OK"});
            buttons.push_back({IDCANCEL, L"Cancel"});
        }
        else if (btnType == MB_YESNO)
        {
            buttons.push_back({IDYES, L"Yes"});
            buttons.push_back({IDNO, L"No"});
        }
        else if (btnType == MB_YESNOCANCEL)
        {
            buttons.push_back({IDYES, L"Yes"});
            buttons.push_back({IDNO, L"No"});
            buttons.push_back({IDCANCEL, L"Cancel"});
        }
        else
        {
            buttons.push_back({IDOK, L"OK"});
        }

        const int btnW = 90;
        const int btnGap = 10;
        int totalBtnW = buttons.size() * btnW + (buttons.size() - 1) * btnGap;
        int btnY = dlgH - button_pane_height + button_margin;
        int currentX = dlgW - button_margin - totalBtnW;

        for (size_t i = 0; i < buttons.size(); ++i)
        {
            const auto &btn = buttons[i];
            bool isDefault = false;
            UINT defBtn = (params->type & MB_DEFMASK);
            if ((defBtn == MB_DEFBUTTON1 && i == 0) || (defBtn == MB_DEFBUTTON2 && i == 1) || (defBtn == MB_DEFBUTTON3 && i == 2))
            {
                isDefault = true;
            }

            HWND hBtn = CreateWindowW(L"BUTTON", btn.second, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW | (isDefault ? BS_DEFPUSHBUTTON : 0), currentX, btnY, btnW, btnH, hwnd, (HMENU)btn.first, g_hInst, NULL);
            SendMessage(hBtn, WM_SETFONT, (WPARAM)hFontText, TRUE);
            if (isDefault)
                SetFocus(hBtn);
            currentX += btnW + btnGap;
        }

        RECT wndRect = {0, 0, dlgW, dlgH};
        AdjustWindowRect(&wndRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);
        int finalW = wndRect.right - wndRect.left;
        int finalH = wndRect.bottom - wndRect.top;

        int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - finalW) / 2, y = (screenH - finalH) / 2;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, finalW, finalH, SWP_SHOWWINDOW);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, DARK_TEXT);
        return (LRESULT)hbrBg;
    }
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);

        const int btnH = 32;
        const int button_margin = 10;
        const int button_pane_height = btnH + 2 * button_margin;

        RECT rcContent = rc;
        rcContent.bottom = rc.bottom - button_pane_height;

        RECT rcButtonPane = rc;
        rcButtonPane.top = rcContent.bottom;

        FillRect(hdc, &rcContent, hbrBg);
        FillRect(hdc, &rcButtonPane, hbrButtonPaneBg);

        return 1;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        bool isPrimary = (dis->CtlID == IDOK || dis->CtlID == IDYES);
        DrawThemedButton(dis, isPrimary);
        return TRUE;
    }
    case WM_COMMAND:
        if (params && params->result)
        {
            *(params->result) = LOWORD(wParam);
        }
        DestroyWindow(hwnd);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            if (params && params->result)
            {
                UINT btnType = params->type & 0x0F;
                if (btnType == MB_OKCANCEL || btnType == MB_YESNOCANCEL)
                    *(params->result) = IDCANCEL;
            }
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        if (params && params->result)
        {
            UINT btnType = params->type & 0x0F;
            if (btnType == MB_OKCANCEL || btnType == MB_YESNOCANCEL)
                *(params->result) = IDCANCEL;
        }
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (hFontText)
        {
            DeleteObject(hFontText);
            hFontText = NULL;
        }
        if (hbrBg)
        {
            DeleteObject(hbrBg);
            hbrBg = NULL;
        }
        if (hbrButtonPaneBg)
        {
            DeleteObject(hbrButtonPaneBg);
            hbrButtonPaneBg = NULL;
        }
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int ShowDarkMessageBox(HWND owner, const wchar_t *text, const wchar_t *caption, UINT type)
{
    const wchar_t DLG_CLASS_NAME[] = L"AntiAFK-RBX-DarkMessageBox";
    WNDCLASS wc = {0};
    if (!GetClassInfo(g_hInst, DLG_CLASS_NAME, &wc))
    {
        wc.lpfnWndProc = DarkMessageBoxProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = DLG_CLASS_NAME;
        wc.hbrBackground = CreateSolidBrush(DARK_BG);
        wc.hIcon = CreateCustomIcon();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        if (!RegisterClass(&wc))
            return 0;
    }

    int result = 0;
    UINT btnType = type & 0x0F;
    if (btnType == MB_OKCANCEL || btnType == MB_YESNOCANCEL)
        result = IDCANCEL;
    else
        result = IDOK;

    DarkMessageBoxParams params = {text, type, &result};

    HWND hDialog = CreateWindowEx(WS_EX_TOPMOST, DLG_CLASS_NAME, caption, WS_POPUP | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, owner, NULL, g_hInst, &params);
    if (hDialog)
    {
        if (owner)
            EnableWindow(owner, FALSE);
        MSG msg;
        while (IsWindow(hDialog) && GetMessage(&msg, NULL, 0, 0))
        {
            if (!IsDialogMessage(hDialog, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        if (owner)
        {
            EnableWindow(owner, TRUE);
            SetForegroundWindow(owner);
        }
    }
    return result;
}

void main_thread()
{
    UpdateSplashStatus(L"Checking for updates...");
    if (g_autoUpdate.load())
    {
        try
        {
            CheckForUpdates();
        }
        catch (...)
        {
        }
    }

    UpdateSplashStatus(L"Preparing user-safe mode...");
    if (g_userSafe.load())
    {
        StartActivityMonitor();
    }

    UpdateSplashStatus(L"Checking for announcements...");
    try
    {
        CheckForAnnouncement();
    }
    catch (...)
    {
    }

    UpdateSplashStatus(L"Finalizing setup...");
    bool wasAutoStarted = false;
    if (g_autoStartAfk.load())
    {
        auto wins = FindAllRobloxWindows(true);
        if (!wins.empty())
        {
            UpdateSplashStatus(L"Auto-starting Anti-AFK...");
            g_isAfkStarted = true;
            CreateTrayMenu(true);
            UpdateTrayIcon();
            wasAutoStarted = true;
            ShowTrayNotification(L"AntiAFK-RBX | Auto-Started", L"AntiAFK has started automatically.");
        }
        else
        {
            UpdateSplashStatus(L"Roblox not found for auto-start.");
            wasAutoStarted = true;
            ShowTrayNotification(L"AntiAFK-RBX | Auto-Start Error", L"Roblox window not found! AntiAFK is not started!");
        }
    }

    Sleep(500);
    UpdateSplashStatus(L"Loaded!");
    Sleep(1200);
    if (g_hSplashWnd)
    {
        PostMessage(g_hSplashWnd, WM_CLOSE, 0, 0);
    }

    if (!g_tutorialShown.load())
        PostMessage(g_hwnd, WM_APP + 2, 0, 0);

    if (!wasAutoStarted)
        ShowTrayNotification(L"AntiAFK-RBX | Welcome", L"The program is running, to change the settings, click on the button in the tray");

    while (!g_stopThread.load())
    {
        std::unique_lock<std::mutex> lock(g_cv_m);
        if (g_isAfkStarted.load())
        {
            HWND user = GetForegroundWindow();
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                if (g_autoStartAfk.load())
                {
                    g_isAfkStarted = false;
                    CreateTrayMenu(false);
                    UpdateTrayIcon();
                    ShowTrayNotification(L"AntiAFK-RBX | Auto-Stopped", L"Roblox window not found. AntiAFK is now off.");
                    continue;
                }
                else
                {
                    ShowTrayNotification(L"AntiAFK Error", L"Roblox window not found!");
                }
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

                if (g_multiSupport.load())
                {
                    for (HWND w : wins)
                    {
                        bool wasMinimized = IsIconic(w);
                        if (wasMinimized)
                            ShowWindow(w, SW_RESTORE);

                        SetForegroundWindow(w);
                        for (int j = 0; j < 3; j++)
                        {
                            AntiAFK_Action(w);
                        }

                        if (wasMinimized)
                            ShowWindow(w, SW_MINIMIZE);
                    }
                }
                else
                {
                    HWND w = wins.front();
                    bool wasMinimized = IsIconic(w);
                    if (wasMinimized)
                        ShowWindow(w, SW_RESTORE);

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
                RestoreForegroundWindow(user);
            }
            g_updateInterval = false;
            g_cv.wait_for(lock, std::chrono::seconds(g_selectedTime), []
                          { return g_stopThread.load() || g_updateInterval.load(); });
        }
        else
        {
            if (g_autoStartAfk.load())
            {
                auto wins = FindAllRobloxWindows(true);
                if (!wins.empty())
                {
                    g_isAfkStarted = true;
                    CreateTrayMenu(true);
                    UpdateTrayIcon();
                    ShowTrayNotification(L"AntiAFK-RBX | Auto-Started", L"AntiAFK has started automatically.");
                    continue;
                }
            }
            g_cv.wait_for(lock, 5000ms, []
                          { return g_stopThread.load() || g_isAfkStarted.load(); });
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
    case WM_APP + 2:
        ShowTutorialDialog(hwnd);
        return 0;
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
        case ID_ABOUT_MENU:
        {
            ShowAboutDialog(hwnd);
            break;
        }
        case ID_SHOW_TUTORIAL:
        {
            ShowTutorialDialog(hwnd);
            break;
        }
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
                g_cv.notify_all();
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
            g_cv.notify_all();
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
                            ShowWindow(w, SW_RESTORE);

                        SetForegroundWindow(w);
                        for (int j = 0; j < 3; j++)
                        {
                            AntiAFK_Action(w);
                        }

                        if (wasMinimized)
                            ShowWindow(w, SW_MINIMIZE);
                    }
                }
                else
                {
                    HWND w = wins.front();
                    bool wasMinimized = IsIconic(w);
                    if (wasMinimized)
                        ShowWindow(w, SW_RESTORE);

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
                try
                {
                    CheckForUpdates();
                }
                catch (...)
                {
                }
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
        case ID_AUTO_START_AFK:
            g_autoStartAfk = !g_autoStartAfk.load();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_RESET_SETTINGS:
        {
            int result = ShowDarkMessageBox(hwnd, L"Are you sure you want to reset all settings to their defaults?\nThis action cannot be undone.", L"Reset Settings", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            if (result == IDYES)
            {
                ResetSettings();
                CreateTrayMenu(g_isAfkStarted.load());
                UpdateTrayIcon();
                ShowTrayNotification(L"Settings Reset", L"All settings have been restored to their defaults.");
            }
            break;
        }
        case ID_TIME_CUSTOM:
            ShowCustomIntervalDialog(g_hwnd);
            break;
        case ID_TIME_3:
        case ID_TIME_6:
        case ID_TIME_9:
        case ID_TIME_11:
        case ID_TIME_13:
        case ID_TIME_15:
        case ID_TIME_18:
        {
            int newTime = g_selectedTime;
            switch (LOWORD(wParam))
            {
            case ID_TIME_3:
                newTime = 3 * 60;
                break;
            case ID_TIME_6:
                newTime = 6 * 60;
                break;
            case ID_TIME_9:
                newTime = 9 * 60;
                break;
            case ID_TIME_11:
                newTime = 11 * 60;
                break;
            case ID_TIME_13:
                newTime = 13 * 60;
                break;
            case ID_TIME_15:
                newTime = 15 * 60;
                break;
            case ID_TIME_18:
                newTime = 18 * 60;
                break;
            }
            if (g_selectedTime != newTime)
            {
                g_selectedTime = newTime;
                g_updateInterval = true;
                g_cv.notify_all();
                SaveSettings();
                CreateTrayMenu(g_isAfkStarted.load());
            }
            break;
        }
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
    if (FindWindow(CLASS_NAME, NULL))
    {
        ShowDarkMessageBox(NULL, L"AntiAFK-RBX is already running.\nCheck the system tray (next to the clock).", L"AntiAFK-RBX", MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    LoadSettings();
    CreateSplashScreen(hInstance);
    RegisterClass(&wc);
    g_hwnd = CreateWindowEx(0, CLASS_NAME, L"AntiAFK-RBX-tray", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (g_multiSupport.load())
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