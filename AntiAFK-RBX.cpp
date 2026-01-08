// AntiAFK-RBX.cpp | The best program for AntiAFK and Multi-Instance in Roblox. Or just Roblox Anti-AFK. | By Agzes
// https://github.com/Agzes/AntiAFK-RBX • \[=_=]/


int currentVersion = 30002; // Major*10000 + Minor*100 + Patch or Mini Update
const wchar_t* g_Version = L"v.3 [30002]";


#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <ShlObj.h>
#include "resource.h"
#include <tlhelp32.h>
#include <string>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <dwmapi.h>
#include <cmath> 
#include <winreg.h>
#include <WinInet.h>
#include <gdiplus.h>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <io.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "WinInet.lib")


using namespace Gdiplus;
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
#define ID_USER_SAFE_SUBMENU 303 
#define ID_USER_SAFE_OFF 304
#define ID_USER_SAFE_LEGACY 305
#define ID_USER_SAFE_BETA 306
#define ID_AUTO_START_AFK 307
#define ID_RESET_SETTINGS 308
#define ID_AUTO_RECONNECT 309

#define ID_USE_LEGACY_UI 310
#define ID_BLOXSTRAP_INTEGRATION 311
#define ID_RESTORE_METHOD_SUBMENU 400
#define ID_UNLOCK_FPS_ON_FOCUS 312
#define ID_RESTORE_OFF 401
#define ID_RESTORE_FOREGROUND 402
#define ID_RESTORE_ALTTAB 403

#define ID_FPS_CAPPER_SUBMENU 500
#define ID_FPS_CAP_OFF 501
#define ID_FPS_CAP_3 502
#define ID_FPS_CAP_5 503
#define ID_FPS_CAP_7 504
#define ID_FPS_CAP_10 505
#define ID_FPS_CAP_15 506

#define ID_FPS_CAP_CUSTOM_BASE 600 
#define ID_FPS_CAP_CUSTOM 507

#define ID_UPDATE_AVAILABLE 1000
#define ID_ANNOUNCEMENT_TEXT 1001

#define ID_EDIT_SECONDS 1100
#define ID_BTN_GITHUB 1101
#define ID_BTN_CLOSE_ABOUT 1102
#define ID_BTN_NEXT 1201
#define ID_BTN_BACK 1202
#define ID_BTN_SKIP 1203
#define ID_OPEN_UI 1301


HWND g_hwnd;
HINSTANCE g_hInst;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
HANDLE g_hMultiInstanceMutex = NULL;
HWND g_hSplashWnd = NULL;
HWND g_hMainUiWnd = NULL;
HWND g_hCustomFpsWnd = NULL;
HWND g_hCustomIntervalWnd = NULL;
std::atomic<bool> g_isAfkStarted(false), g_stopThread(false), g_multiSupport(false), g_fishstrapSupport(false), g_autoUpdate(true), g_updateFound(false), g_autoStartAfk(false), g_autoReconnect(false), g_userActive(false), g_monitorThreadRunning(false), g_updateInterval(false), g_tutorialShown(false), g_useLegacyUi(false), g_unlockFpsOnFocus(false), g_notificationsDisabled(false), g_bloxstrapIntegration(false), g_isFpsCapperRunning(false),  g_isFpsCapperPaused(false);
std::atomic<uint64_t> g_lastActivityTime(0), g_afkStartTime(0), g_lastAfkActionTimestamp(0), g_autoReconnectsPerformed(0), g_afkActionsPerformed(0), g_totalAfkTimeSeconds(0);
std::thread g_activityMonitorThread;
std::thread g_fpsCapperThread;
std::condition_variable g_cv;
std::vector<DWORD> g_manuallyStoppedPids;
std::mutex g_cv_m;

const TCHAR g_szClassName[] = _T("AntiAFK-RBX-tray");
wchar_t g_splashStatus[128] = L"Initializing...";
constexpr DWORD ACTION_DELAY = 30, ALT_DELAY = 15;
int g_selectedTime = 540;
int g_selectedAction = 0; // 0 - space, 1 - w&s, 2 - zoom
int g_restoreMethod = 1; // 0 - Off, 1 - SetForeground, 2 - Alt+Tab
int g_userSafeMode = 0; // 0 - Off, 1 - Legacy, 2 - Beta
int g_fpsLimit = 0; // 0 for OFF
const int USER_INACTIVITY_WAIT = 3;
const int MAX_WAIT_TIME = 60;
const COLORREF DARK_BG = RGB(30, 30, 30);
const COLORREF DARK_TEXT = RGB(235, 235, 235);
const COLORREF DARK_EDIT_BG = RGB(45, 45, 45);


bool CheckForUpdates(bool showNotification);

// Args
void ShowHelp()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole())
    {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        _setmode(_fileno(stdout), _O_WTEXT);

        const wchar_t* helpText = L""
            L"AntiAFK-RBX - Command Line Help\n"
            L"================================================\n\n"
            L"Usage: AntiAFK-RBX.exe [options]\n\n"
            L"Startup & UI:\n"
            L"  --no-splash\t\t\t\tSkip the splash screen on startup.\n"
            L"  --tray\t\t\t\tStart minimized to the system tray without showing any UI.\n"
            L"  --legacy-ui\t\t\t\tForce the old tray-only context menu UI.\n"
            L"  --no-notifications\t\t\tDisable all non-critical tray notifications.\n"
            L"  --no-update-check\t\t\tSkip checking for updates on startup.\n"
            L"  --help, -?\t\t\t\tShow this help message and exit.\n\n"

            L"Anti-AFK Control:\n"
            L"  --start-afk\t\t\t\tStart the Anti-AFK function immediately on launch.\n"
            L"  --set-interval <seconds>\t\tSet the AFK interval in seconds (e.g., --set-interval 300).\n"
            L"  --set-action <action>\t\t\tSet the AFK action. Values: space, ws, zoom.\n\n"

            L"Settings Override (these override saved settings for the current session):\n"
            L"  --multi-instance [on|off]\t\tForce enable or disable multi-instance support.\n"
            L"  --auto-start [on|off]\t\t\tForce enable or disable the 'Auto-Start' feature.\n"
            L"  --auto-reconnect [on|off]\t\tForce enable or disable the 'Auto Reconnect' feature.\n"
            L"  --user-safe <mode>\t\t\tSet User-Safe mode. Values: off, legacy, beta.\n"
            L"  --restore-window <method>\t\tSet window restore method. Values: off, foreground, alttab.\n"
            L"  --fishstrap-support [on|off]\t\tForce enable or disable support for modified clients.\n"
            L"  --bloxstrap-integration\t\tEnable Bloxstrap/Fishstrap integration. If an instance is already running, it will exit gracefully.\n"
            L"  --unlock-fps-on-focus [on|off]\tForce enable or disable unlocking FPS when Roblox is focused.\n"
            L"  --set-fps-limit <fps>\t\t\tSet the FPS limit (0 to disable).\n\n"


            L"================================================\n";

        std::wcout << helpText << std::endl;
        FreeConsole();
    }
}
// ==========

// UI/Tray Utils
int ShowDarkMessageBox(HWND owner, const wchar_t* text, const wchar_t* caption, UINT type);
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef enum _WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
} WINDOWCOMPOSITIONATTRIB;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBDATA;
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
void EnableAcrylic(HWND hWnd) {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
        pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hUser32, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            DWORD acrylicColor = 0x301A1A1A;
           

            ACCENT_POLICY accent = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, acrylicColor, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &accent, sizeof(accent) };
            SetWindowCompositionAttribute(hWnd, &data);
        }
    }
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hWnd, &margins);
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
void ShowTrayNotification(const wchar_t* title, const wchar_t* msg)
{
    if (g_notificationsDisabled.load()) return;

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
HICON CreateCustomIconWithState(bool isOn, bool multi)
{
    int iconId = IDI_TRAY_OFF;
    if (isOn) {
        iconId = multi ? IDI_TRAY_ON_MULTI : IDI_TRAY_ON;
    }
    return (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(iconId), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
}
HICON CreateCustomIcon()
{
    return CreateCustomIconWithState(g_isAfkStarted.load(), g_multiSupport.load());
}
// ==========

// Splash
struct SplashData {
    HFONT hFont28 = NULL, hFont12b = NULL, hFont12 = NULL, hFont10 = NULL;
    RECT closeButtonRect = { 0 };
    bool isHoveringClose = false;
    bool isTrackingMouse = false;
    UINT_PTR uTimerId = 0;
};
void DrawSharedCloseButton(Graphics* g, const RECT& closeButtonRect, bool isHovering) {
    if (isHovering) {
        SolidBrush redBrush(Color(255, 232, 17, 35));
        g->FillRectangle(&redBrush, (REAL)closeButtonRect.left, (REAL)closeButtonRect.top, (REAL)(closeButtonRect.right - closeButtonRect.left), (REAL)(closeButtonRect.bottom - closeButtonRect.top));
    }

    int cx = (closeButtonRect.left + closeButtonRect.right) / 2;
    int cy = (closeButtonRect.top + closeButtonRect.bottom) / 2;
    int size = 5;

    Pen pen(Color(255, 255, 255, 255), 1.0f);
    REAL sizeF = (REAL)size;
    g->DrawLine(&pen, (REAL)cx - sizeF, (REAL)cy - sizeF, (REAL)cx + sizeF, (REAL)cy + sizeF);
    g->DrawLine(&pen, (REAL)cx + sizeF, (REAL)cy - sizeF, (REAL)cx - sizeF, (REAL)cy + sizeF);
}

void Splash_Paint_DrawCloseButton(HDC hdc, const RECT& closeButtonRect, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    DrawSharedCloseButton(&g, closeButtonRect, isHovering);
}
void Splash_Paint_DrawTitle(HDC hdc, const RECT& clientRect, HFONT hFont28, HFONT hFont12) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    const wchar_t* part1 = L"AntiAFK-";
    const wchar_t* part2 = L"RBX";
    
    Font font28(hdc, hFont28);
    RectF s1, s2;
    PointF origin(0,0);
    const StringFormat* typographic = StringFormat::GenericTypographic();
    
    g.MeasureString(part1, -1, &font28, origin, typographic, &s1);
    g.MeasureString(part2, -1, &font28, origin, typographic, &s2);

    REAL totalWidth = s1.Width + s2.Width; 
    REAL startX = ((REAL)clientRect.right - totalWidth) / 2.0f;
    REAL y_title = ((REAL)clientRect.bottom / 2.0f) - 50.0f;

    SolidBrush whiteBrush(Color(255, 255, 255, 255));
    SolidBrush redBrush(Color(255, 226, 35, 26));

    g.DrawString(part1, -1, &font28, PointF(startX, y_title), typographic, &whiteBrush);
    g.DrawString(part2, -1, &font28, PointF(startX + s1.Width, y_title), typographic, &redBrush);

    Font font12(hdc, hFont12);
    SolidBrush grayBrush(Color(255, 123, 123, 123));
    
    StringFormat sfCenter;
    sfCenter.SetAlignment(StringAlignmentCenter);
    RectF noteRectF((REAL)0, (REAL)y_title + s1.Height, (REAL)clientRect.right, (REAL)clientRect.bottom);
    g.DrawString(g_splashStatus, -1, &font12, noteRectF, &sfCenter, &grayBrush);
}
void Splash_Paint_DrawFooter(HDC hdc, const RECT& clientRect, HFONT hFont10, HFONT hFont12) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RECT by_rect = { clientRect.left + 10, clientRect.top, clientRect.right, clientRect.bottom - 7 };
    Font font10(hdc, hFont10);
    SolidBrush grayBrush(Color(255, 137, 137, 137));
    StringFormat sfLeftBottom;
    sfLeftBottom.SetAlignment(StringAlignmentNear);
    sfLeftBottom.SetLineAlignment(StringAlignmentFar);
    RectF byRectF((REAL)by_rect.left, (REAL)by_rect.top, (REAL)(by_rect.right - by_rect.left), (REAL)(by_rect.bottom - by_rect.top));
    g.DrawString(L"by Agzes", -1, &font10, byRectF, &sfLeftBottom, &grayBrush);

    RECT version_rect = { clientRect.left, clientRect.top, clientRect.right - 10, clientRect.bottom - 7 };
    StringFormat sfRightBottom;
    sfRightBottom.SetAlignment(StringAlignmentFar);
    sfRightBottom.SetLineAlignment(StringAlignmentFar);
    RectF verRectF((REAL)version_rect.left, (REAL)version_rect.top, (REAL)(version_rect.right - version_rect.left), (REAL)(version_rect.bottom - version_rect.top));
    g.DrawString(g_Version, -1, &font10, verRectF, &sfRightBottom, &grayBrush);

    const wchar_t* loadingStr = L"LOADING";
    RECT loading_rect = { clientRect.left, clientRect.top, clientRect.right, clientRect.bottom - 7 };
    
    float sine_wave = (sin(GetTickCount64() * 0.003f) + 1.0f) / 2.0f;
    int brightness = 120 + (int)(sine_wave * 100);
    SolidBrush loadingBrush(Color(255, brightness, brightness, brightness));
    Font font12(hdc, hFont12);
    StringFormat sfCenterBottom;
    sfCenterBottom.SetAlignment(StringAlignmentCenter);
    sfCenterBottom.SetLineAlignment(StringAlignmentFar);
    RectF loadRectF((REAL)loading_rect.left, (REAL)loading_rect.top, (REAL)(loading_rect.right - loading_rect.left), (REAL)(loading_rect.bottom - loading_rect.top));
    g.DrawString(loadingStr, -1, &font12, loadRectF, &sfCenterBottom, &loadingBrush);
}
LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SplashData* pData = (SplashData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new SplashData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        pData->hFont28 = CreateFontW(-MulDiv(28, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        pData->hFont12b = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        pData->hFont12 = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont10 = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        pData->closeButtonRect.left = clientRect.right - 46;
        pData->closeButtonRect.top = 0;
        pData->closeButtonRect.right = clientRect.right;
        pData->closeButtonRect.bottom = 32;

        enum DWM_WINDOW_CORNER_PREFERENCE {
            DWMWCP_DEFAULT = 0,
            DWMWCP_DONOTROUND = 1,
            DWMWCP_ROUND = 2,
            DWMWCP_ROUNDSMALL = 3
        };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        pData->uTimerId = timeSetEvent(16, 1, [](UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
            if (IsWindow((HWND)dwUser)) {
                InvalidateRect((HWND)dwUser, NULL, FALSE);
            }
            }, (DWORD_PTR)hwnd, TIME_PERIODIC);

        return 0;
    }
    case WM_APP + 1:
        lstrcpynW(g_splashStatus, (const wchar_t*)wParam, 128);
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        return 0;
    case WM_MOUSEMOVE:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool nowHovering = PtInRect(&pData->closeButtonRect, pt);
        if (nowHovering != pData->isHoveringClose)
        {
            pData->isHoveringClose = nowHovering;
            InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        }

        if (nowHovering && !pData->isTrackingMouse)
        {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        break;
    }
    case WM_MOUSELEAVE: {
        if (pData->isHoveringClose)
        {
            pData->isHoveringClose = false;
            InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        }
        pData->isTrackingMouse = false;
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt))
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt))
        {
            ShowWindow(hwnd, SW_MINIMIZE);
            return 0;
        }
        break;
    }
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);

            RECT dragRect = { 0, 0, pData->closeButtonRect.left, 32 };
            if (PtInRect(&dragRect, pt))
            {
                return HTCAPTION;
            }
        }
        return hit;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        FillRect(memDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        SetBkMode(memDC, TRANSPARENT);

        Splash_Paint_DrawCloseButton(memDC, pData->closeButtonRect, pData->isHoveringClose);
        Splash_Paint_DrawTitle(memDC, clientRect, pData->hFont28, pData->hFont12);
        Splash_Paint_DrawFooter(memDC, clientRect, pData->hFont10, pData->hFont12);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);

        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY: {
        if (pData) {
            if (pData->uTimerId != 0) {
                timeKillEvent(pData->uTimerId);
            }
            if (pData->hFont28) DeleteObject(pData->hFont28);
            if (pData->hFont12b) DeleteObject(pData->hFont12b);
            if (pData->hFont12) DeleteObject(pData->hFont12);
            if (pData->hFont10) DeleteObject(pData->hFont10);
            delete pData;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        g_hSplashWnd = NULL;
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
void UpdateSplashStatus(const wchar_t* status)
{
    if (g_hSplashWnd)
        PostMessage(g_hSplashWnd, WM_APP + 1, (WPARAM)status, 0);
}
void CreateSplashScreen(HINSTANCE hInstance)
{
    const wchar_t SPLASH_CLASS_NAME[] = L"AntiAFK-RBX-Splash";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = SplashWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = SPLASH_CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(DARK_BG);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
    if (!RegisterClass(&wc))
        return;

    int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 286, winH = 312;
    int x = (screenW - winW) / 2, y = (screenH - winH) / 2;

    g_hSplashWnd = CreateWindowEx(WS_EX_TOPMOST, SPLASH_CLASS_NAME, L"AntiAFK-RBX", WS_POPUP, x, y, winW, winH, NULL, NULL, hInstance, NULL);

    if (g_hSplashWnd)
    {
        EnableAcrylic(g_hSplashWnd);
        AnimateWindow(g_hSplashWnd, 200, AW_BLEND);
        UpdateWindow(g_hSplashWnd);
    }
}
// ==========

// About Window
struct AboutData {
    HFONT hFont28 = NULL, hFont12b = NULL, hFont12 = NULL, hFont10 = NULL;
    RECT closeButtonRect = { 0 };
    RECT githubButtonRect = { 0 }, sourceforgeButtonRect = { 0 };
    RECT topCloseButtonRect = { 0 };
    RECT updateButtonRect = { 0 };
    bool isHoveringGithub = false;
    bool isHoveringSourceforge = false;
    bool isTrackingMouse = false;
    bool isHoveringTopClose = false;
    bool isHoveringUpdate = false;
    HCURSOR hCursorHand = NULL;
    HCURSOR hCursorArrow = NULL;
    std::wstring updateButtonText = L"Check for Updates";
    int updateCheckState = 0; // 0: idle, 1: checking, 2: found, 3: not found/error
    COLORREF updateButtonCurrentColor = RGB(160, 160, 160);
    UINT_PTR uTimerId = 0;
};

void About_Paint_DrawCloseButton(HDC hdc, const RECT& closeButtonRect, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    DrawSharedCloseButton(&g, closeButtonRect, isHovering);
}
void About_Paint_DrawButtons(HDC hdc, const RECT& githubButtonRect, const RECT& sourceforgeButtonRect, bool isHoveringGithub, bool isHoveringSourceforge, HFONT hFont) {
    auto drawButton = [&](const RECT& rect, const wchar_t* text, bool isHovering, Color hoverColor) {
        Graphics g(hdc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        if (isHovering) {
            SolidBrush bgBrush(hoverColor);
            GraphicsPath path;
            path.AddArc((REAL)rect.left, (REAL)rect.top, 8.0f, 8.0f, 180, 90);
            path.AddArc((REAL)rect.right - 8.0f, (REAL)rect.top, 8.0f, 8.0f, 270, 90);
            path.AddArc((REAL)rect.right - 8.0f, (REAL)rect.bottom - 8.0f, 8.0f, 8.0f, 0, 90);
            path.AddArc((REAL)rect.left, (REAL)rect.bottom - 8.0f, 8.0f, 8.0f, 90, 90);
            path.CloseFigure();
            g.FillPath(&bgBrush, &path);
        } else {
            SolidBrush bgBrush(Color(40, 255, 255, 255));
            GraphicsPath path;
            path.AddArc((REAL)rect.left, (REAL)rect.top, 8.0f, 8.0f, 180, 90);
            path.AddArc((REAL)rect.right - 8.0f, (REAL)rect.top, 8.0f, 8.0f, 270, 90);
            path.AddArc((REAL)rect.right - 8.0f, (REAL)rect.bottom - 8.0f, 8.0f, 8.0f, 0, 90);
            path.AddArc((REAL)rect.left, (REAL)rect.bottom - 8.0f, 8.0f, 8.0f, 90, 90);
            path.CloseFigure();
            g.FillPath(&bgBrush, &path);
        }
        
        Font font(hdc, hFont);
        SolidBrush textBrush(isHovering ? Color(255, 255, 255, 255) : Color(255, 200, 200, 200));
        StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);
        RectF rectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
        g.DrawString(text, -1, &font, rectF, &sf, &textBrush);
    };

    drawButton(githubButtonRect, L"GitHub", isHoveringGithub, Color(255, 45, 51, 57));
    drawButton(sourceforgeButtonRect, L"SourceForge", isHoveringSourceforge, Color(255, 244, 121, 34));
}
void About_Paint_DrawContent(HDC hdc, const RECT& clientRect, HFONT hFont28, HFONT hFont12, HFONT hFont10, HFONT hFont12b) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RECT headerRect = { clientRect.left, clientRect.top, clientRect.right, clientRect.top + 90 };

    SolidBrush bgBrush(Color(100, 10, 10, 10)); 
    g.FillRectangle(&bgBrush, (REAL)clientRect.left, (REAL)headerRect.bottom, (REAL)(clientRect.right - clientRect.left), (REAL)(clientRect.bottom - headerRect.bottom));

    SolidBrush headerBrush(Color(180, 25, 25, 25)); 
    g.FillRectangle(&headerBrush, (REAL)headerRect.left, (REAL)headerRect.top, (REAL)(headerRect.right - headerRect.left), (REAL)(headerRect.bottom - headerRect.top));

    Pen accentPen(Color(255, 226, 35, 26), 1);
    g.DrawLine(&accentPen, (REAL)headerRect.left, (REAL)headerRect.bottom, (REAL)headerRect.right, (REAL)headerRect.bottom);

    Font font28(hdc, hFont28);
    const wchar_t* part1 = L"AntiAFK-";
    const wchar_t* part2 = L"RBX";
    
    RectF s1, s2;
    PointF origin(0,0);
    g.MeasureString(part1, -1, &font28, origin, &s1);
    g.MeasureString(part2, -1, &font28, origin, &s2);
    
    REAL x_title = 20;
    REAL y_title = ((REAL)headerRect.bottom - s1.Height) / 2 - 5;
    
    SolidBrush whiteBrush(Color(255, 255, 255, 255));
    SolidBrush redBrush(Color(255, 226, 35, 26));
    
    g.DrawString(part1, -1, &font28, PointF(x_title, y_title), &whiteBrush);
    g.DrawString(part2, -1, &font28, PointF(x_title + s1.Width * 0.9f, y_title), &redBrush);

    Font font12(hdc, hFont12);
    SolidBrush grayBrush(Color(255, 160, 160, 160));
    g.DrawString(g_Version, -1, &font12, PointF(x_title, y_title + s1.Height - 6), &grayBrush);

    int contentY = headerRect.bottom + 30;
    
    Font font12_c(hdc, hFont12);
    SolidBrush textBrush(Color(255, 220, 220, 220));
    RectF descRectF((REAL)20, (REAL)contentY, (REAL)(clientRect.right - 20), (REAL)50);
    g.DrawString(L"The program for AntiAFK and Multi-Instance in Roblox.", -1, &font12_c, descRectF, NULL, &textBrush);

    Font font12b_c(hdc, hFont12b);
    RectF authorRectF((REAL)20, (REAL)contentY + 60, (REAL)(clientRect.right - 20), (REAL)100);
    g.DrawString(L"Developed by Agzes with \u2764\nLicensed under the MIT License", -1, &font12_c, authorRectF, NULL, &grayBrush);
}
void About_Animation_TimerProc(HWND hwnd, AboutData* pData) {
    if (!pData) return;

    COLORREF targetColor;
    switch (pData->updateCheckState) {
    case 1: targetColor = RGB(180, 180, 180); break;
    case 2: targetColor = RGB(252, 142, 54); break; 
    case 3: targetColor = RGB(0, 150, 136); break;
    default: targetColor = pData->isHoveringUpdate ? RGB(255, 255, 255) : RGB(160, 160, 160); break;
    }

    if (pData->updateButtonCurrentColor != targetColor) {
        BYTE currentR = GetRValue(pData->updateButtonCurrentColor);
        BYTE currentG = GetGValue(pData->updateButtonCurrentColor);
        BYTE currentB = GetBValue(pData->updateButtonCurrentColor);

        BYTE targetR = GetRValue(targetColor);
        BYTE targetG = GetGValue(targetColor);
        BYTE targetB = GetBValue(targetColor);

        auto lerp = [](BYTE a, BYTE b) {
            return static_cast<BYTE>(a + (b - a) * 0.15f);
        };

        pData->updateButtonCurrentColor = RGB(lerp(currentR, targetR), lerp(currentG, targetG), lerp(currentB, targetB));

        if (abs(currentR - targetR) < 2 && abs(currentG - targetG) < 2 && abs(currentB - targetB) < 2) {
            pData->updateButtonCurrentColor = targetColor;
        }
        InvalidateRect(hwnd, &pData->updateButtonRect, FALSE);
    }
}
LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    AboutData* pData = (AboutData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new AboutData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        EnableAcrylic(hwnd);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        pData->hFont28 = CreateFontW(-MulDiv(28, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        pData->hFont12b = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont12 = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont10 = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int btnWidth = 130, btnHeight = 32, btnGap = 10, margin = 20;
        int totalButtonsWidth = btnWidth * 2 + btnGap;
        int startX = margin;
        int btnY = clientRect.bottom - btnHeight - margin;
        pData->githubButtonRect = { startX, btnY, startX + btnWidth, btnY + btnHeight };
        pData->sourceforgeButtonRect = { clientRect.right - margin - btnWidth, btnY, clientRect.right - margin, btnY + btnHeight };
        
        int updateBtnY = btnY - btnHeight/2 - btnGap - 5; 
        pData->updateButtonRect = { (clientRect.right - (btnWidth + 60)) / 2, updateBtnY, (clientRect.right + (btnWidth + 60)) / 2, updateBtnY + btnHeight };
        pData->topCloseButtonRect = { clientRect.right - 46, 0, clientRect.right, 30 };

        enum DWM_WINDOW_CORNER_PREFERENCE {
            DWMWCP_DEFAULT = 0,
            DWMWCP_DONOTROUND = 1,
            DWMWCP_ROUND = 2,
            DWMWCP_ROUNDSMALL = 3
        };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);

        pData->uTimerId = timeSetEvent(16, 1, [](UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
            if (IsWindow((HWND)dwUser)) {
                About_Animation_TimerProc((HWND)dwUser, (AboutData*)GetWindowLongPtr((HWND)dwUser, GWLP_USERDATA));
            }
        }, (DWORD_PTR)hwnd, TIME_PERIODIC);
        return 0;
    }
    case WM_APP + 3:
        if (pData) {
            pData->updateCheckState = wParam; 
            pData->updateButtonText = (wParam == 2) ? L"Update Found!" : L"You are up to date!";
            InvalidateRect(hwnd, &pData->updateButtonRect, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool nowHoveringGithub = PtInRect(&pData->githubButtonRect, pt);
        if (nowHoveringGithub != pData->isHoveringGithub)
        {
            pData->isHoveringGithub = nowHoveringGithub;
            InvalidateRect(hwnd, &pData->githubButtonRect, FALSE);
        }

        bool nowHoveringSourceforge = PtInRect(&pData->sourceforgeButtonRect, pt);
        if (nowHoveringSourceforge != pData->isHoveringSourceforge)
        {
            pData->isHoveringSourceforge = nowHoveringSourceforge;
            InvalidateRect(hwnd, &pData->sourceforgeButtonRect, FALSE);
        }

        bool nowHoveringTopClose = PtInRect(&pData->topCloseButtonRect, pt);
        if (nowHoveringTopClose != pData->isHoveringTopClose)
        {
            pData->isHoveringTopClose = nowHoveringTopClose;
            InvalidateRect(hwnd, &pData->topCloseButtonRect, FALSE);
        }

        bool nowHoveringUpdate = PtInRect(&pData->updateButtonRect, pt);
        if (nowHoveringUpdate != pData->isHoveringUpdate) {
            pData->isHoveringUpdate = nowHoveringUpdate;
            InvalidateRect(hwnd, &pData->updateButtonRect, FALSE);
        }

        bool anyHover = nowHoveringGithub || nowHoveringSourceforge || nowHoveringTopClose || nowHoveringUpdate;
        SetCursor(anyHover ? pData->hCursorHand : pData->hCursorArrow);

        if (anyHover && !pData->isTrackingMouse)
        {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        break;
    }
    case WM_MOUSELEAVE: {
        if (pData->isHoveringGithub)
        {
            pData->isHoveringGithub = false;
            InvalidateRect(hwnd, &pData->githubButtonRect, FALSE);
        }
        if (pData->isHoveringSourceforge)
        {
            pData->isHoveringSourceforge = false;
            InvalidateRect(hwnd, &pData->sourceforgeButtonRect, FALSE);
        }
        if (pData->isHoveringTopClose)
        {
            pData->isHoveringTopClose = false;
            InvalidateRect(hwnd, &pData->topCloseButtonRect, FALSE);
        }
        if (pData->isHoveringUpdate)
        {
            pData->isHoveringUpdate = false;
            InvalidateRect(hwnd, &pData->updateButtonRect, FALSE);
        }
        pData->isTrackingMouse = false;
        SetCursor(pData->hCursorArrow);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->githubButtonRect, pt))
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        }
        if (PtInRect(&pData->sourceforgeButtonRect, pt))
        {
            ShellExecute(NULL, L"open", L"https://antiafk-rbx.sourceforge.io", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        }
        if (PtInRect(&pData->topCloseButtonRect, pt))
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        if (PtInRect(&pData->updateButtonRect, pt))
        {
            if (pData->updateCheckState == 2) {
                ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            } else if (pData->updateCheckState != 1) { 
                pData->updateCheckState = 1; 
                pData->updateButtonText = L"Checking...";
                InvalidateRect(hwnd, &pData->updateButtonRect, FALSE);

                std::thread([hwnd]() {
                    bool found = CheckForUpdates(false);
                    if (IsWindow(hwnd)) {
                        PostMessage(hwnd, WM_APP + 3, found ? 2 : 3, 0);
                    }
                }).detach();
            }
            return 0;
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->topCloseButtonRect, pt))
        {
            ShowWindow(hwnd, SW_MINIMIZE);
            return 0;
        }
        break;
    }
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT)
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            RECT dragRect = { 0, 0, pData->topCloseButtonRect.left, 30 };
            if (PtInRect(&dragRect, pt))
            {
                return HTCAPTION;
            }
        }
        return hit;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        FillRect(memDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        SetBkMode(memDC, TRANSPARENT);

        About_Paint_DrawContent(memDC, clientRect, pData->hFont28, pData->hFont12, pData->hFont10, pData->hFont12b);
        About_Paint_DrawButtons(memDC, pData->githubButtonRect, pData->sourceforgeButtonRect, pData->isHoveringGithub, pData->isHoveringSourceforge, pData->hFont12);

        {
        {
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            Font font(memDC, pData->hFont12);
            SolidBrush textBrush(Color(255, GetRValue(pData->updateButtonCurrentColor), GetGValue(pData->updateButtonCurrentColor), GetBValue(pData->updateButtonCurrentColor)));
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            RectF rectF((REAL)pData->updateButtonRect.left, (REAL)pData->updateButtonRect.top, (REAL)(pData->updateButtonRect.right - pData->updateButtonRect.left), (REAL)(pData->updateButtonRect.bottom - pData->updateButtonRect.top));
            g.DrawString(pData->updateButtonText.c_str(), -1, &font, rectF, &sf, &textBrush);
        }
        }

        About_Paint_DrawCloseButton(memDC, pData->topCloseButtonRect, pData->isHoveringTopClose);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);

        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY: {
        if (pData) {
            if (pData->uTimerId != 0) {
                timeKillEvent(pData->uTimerId);
            }
            if (pData->hFont28) DeleteObject(pData->hFont28);
            if (pData->hFont12b) DeleteObject(pData->hFont12b);
            if (pData->hFont12) DeleteObject(pData->hFont12);
            if (pData->hFont10) DeleteObject(pData->hFont10);
            if (pData->hCursorHand) DestroyCursor(pData->hCursorHand);
            if (pData->hCursorArrow) DestroyCursor(pData->hCursorArrow);
            delete pData;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
void ShowAboutDialog(HWND owner)
{
    static bool registered = false;
    const wchar_t ABOUT_CLASS_NAME[] = L"AntiAFK-RBX-About";
    if (!registered)
    {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = AboutWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = ABOUT_CLASS_NAME;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN)); 
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 320, winH = 340;
    int x = (screenW - winW) / 2, y = (screenH - winH) / 2;

    HWND h = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, ABOUT_CLASS_NAME,
        L"About AntiAFK-RBX", WS_POPUP,
        x, y, winW, winH, owner, NULL, g_hInst, NULL);

    if (h)
    {
        EnableAcrylic(h);
        ShowWindow(h, SW_SHOW);
        UpdateWindow(h);
    }
}
// ==========

// Enum Windows
struct EnumWindowsData
{
    DWORD processId;
    bool includeHidden;
    std::vector<HWND>* wins;
};
BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lParam)
{
    auto* data = reinterpret_cast<EnumWindowsData*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == data->processId && GetWindowTextLength(h) > 0 &&
        (data->includeHidden || IsWindowVisible(h)))
        data->wins->push_back(h);
    return TRUE;
}
// ==========

// Window utils
std::vector<HWND> GetWindowsForProcess(DWORD processId, bool includeHidden = false)
{
    std::vector<HWND> wins;
    EnumWindowsData data{ processId, includeHidden, &wins };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return wins;
}
std::vector<HWND> FindAllRobloxWindows(bool includeHidden = false)
{
    std::vector<HWND> wins;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return wins;
    PROCESSENTRY32 pe = { 0 };
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
HWND FindWindowByProcessName(const wchar_t* processName)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return NULL;
    PROCESSENTRY32 pe = { 0 };
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
void RestorePreviousWindowWithAltTab()
{
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_TAB, 0, 0, 0);
    keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
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
// ==========

// Multi Instance
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
// ==========

// AntiAFK Action
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
// ==========

// Auto Reconnect Check
void IncrementalMouseMove(int startX, int startY, int endX, int endY, int steps, int duration)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const double PI = 3.14159265358979323846;

    for (int i = 1; i <= steps; ++i)
    {
        double progress = (double)i / steps;
        double factor = 0.5 * (1.0 - cos(progress * PI));

        int currentX = (int)(startX + (endX - startX) * factor);
        int currentY = (int)(startY + (endY - startY) * factor);

        long absoluteX = currentX * 65535 / screenWidth;
        long absoluteY = currentY * 65535 / screenHeight;

        mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, absoluteX, absoluteY, 0, 0);

        if (steps > 1 && duration > 0) {
            Sleep(duration / steps);
        }
    }

}
bool CheckForAutoReconnect(HWND hRobloxWnd)
{
    if (!IsWindow(hRobloxWnd) || !IsWindowVisible(hRobloxWnd)) {
        return false;
    }

    BringWindowToTop(hRobloxWnd);
    SetForegroundWindow(hRobloxWnd);
    Sleep(250); 
    RECT clientRect;
    if (!GetClientRect(hRobloxWnd, &clientRect)) {
        return false;
    }
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;

    const int kickWidth = 400;
    const int kickHeight = 250;
    int elemX = (windowWidth - kickWidth) / 2;
    int elemY = (windowHeight - kickHeight) / 2;
    POINT checkPoint = { elemX + 10, elemY + 10 };

    POINT screenCheckPoint = checkPoint;
    ClientToScreen(hRobloxWnd, &screenCheckPoint);

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return false;
    COLORREF pixelColor = GetPixel(hdcScreen, screenCheckPoint.x, screenCheckPoint.y);
    ReleaseDC(NULL, hdcScreen);

    if (pixelColor != RGB(57, 59, 61)) {
        return false; 
    }

    const int btnWidth = 161;
    const int btnHeight = 34;
    int btnRelX = kickWidth - btnWidth - 27;
    int btnRelY = kickHeight - btnHeight - 21;
    int clickX = elemX + btnRelX + (rand() % btnWidth);
    int clickY = elemY + btnRelY + (rand() % btnHeight);

    POINT clickPoint = { clickX, clickY };
    ClientToScreen(hRobloxWnd, &clickPoint);

    POINT oldCursorPos;
    GetCursorPos(&oldCursorPos);

    IncrementalMouseMove(oldCursorPos.x, oldCursorPos.y, clickPoint.x, clickPoint.y, 20, 80); 

    Sleep(30 + rand() % 20);

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(40 + rand() % 30); 
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    SetCursorPos(oldCursorPos.x, oldCursorPos.y);

    return true;
}
// ==========

// Bloxstrap integration
std::string GetBloxstrapSettingsPath() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"roblox-player\\DefaultIcon", 0, KEY_READ | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS) {
        return "";
    }

    wchar_t value[MAX_PATH];
    DWORD size = sizeof(value);
    if (RegQueryValueExW(hKey, NULL, NULL, NULL, (LPBYTE)value, &size) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return "";
    }
    RegCloseKey(hKey);

    std::wstring path_w(value);
    std::wstring exe_path_w;

    size_t first_quote = path_w.find(L'"');
    if (first_quote == 0) { 
        size_t second_quote = path_w.find(L'"', first_quote + 1);
        if (second_quote == std::wstring::npos) { return ""; }
        exe_path_w = path_w.substr(first_quote + 1, second_quote - first_quote - 1);
    } else {
        size_t comma = path_w.find(L',');
        exe_path_w = (comma == std::wstring::npos) ? path_w : path_w.substr(0, comma);
    }

    size_t last_slash = exe_path_w.find_last_of(L"\\/");
    if (last_slash == std::wstring::npos) { return ""; }

    std::wstring dir_path_w = exe_path_w.substr(0, last_slash);
    
    int len = WideCharToMultiByte(CP_UTF8, 0, (dir_path_w + L"\\Settings.json").c_str(), -1, NULL, 0, NULL, NULL);
    std::string settings_path(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, (dir_path_w + L"\\Settings.json").c_str(), -1, &settings_path[0], len, NULL, NULL);
    settings_path.pop_back(); 

    return settings_path;
}
std::string GetSelfExePath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring wpath = path;
    for (auto& c : wpath) {
        if (c == '\\') c = '/';
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, NULL, 0, NULL, NULL);
    std::string spath(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, &spath[0], len, NULL, NULL);
    spath.pop_back();
    return spath;
}
void UpdateBloxstrapIntegration(bool enable) {
    std::string settingsPath = GetBloxstrapSettingsPath();
    if (settingsPath.empty()) {
        ShowTrayNotification(L"Bloxstrap Integration Error", L"Could not find Bloxstrap installation path in registry.");
        g_bloxstrapIntegration = false;
        return;
    }

    std::ifstream inFile(settingsPath, std::ios::binary);
    if (!inFile.is_open()) {
        ShowTrayNotification(L"Bloxstrap Integration Error", L"Could not find/open Bloxstrap's Settings.json file.");
        g_bloxstrapIntegration = false;
        return;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    std::string integrationName = "AntiAFK-RBX \\u2022 AutoLaunch";
    size_t integrationPos = content.find(integrationName);

    if (enable) {
        if (integrationPos != std::string::npos) {
            return;
        }

        std::string selfPath = GetSelfExePath();

        std::string integrationJson = "    {\r\n      \"Name\": \"" + integrationName + "\",\r\n      \"Location\": \"" + selfPath + "\",\r\n      \"LaunchArgs\": \"--bloxstrap-integration\",\r\n      \"Delay\": 0,\r\n      \"PreLaunch\": false,\r\n      \"AutoClose\": true\r\n    }";

        size_t integrationsArrayPos = content.find("\"CustomIntegrations\": [");
        if (integrationsArrayPos == std::string::npos) {
            ShowTrayNotification(L"Bloxstrap Integration Error", L"Could not find 'CustomIntegrations' array in Settings.json.");
            g_bloxstrapIntegration = false;
            return;
        }

        size_t openingBracket = content.find('[', integrationsArrayPos);
        size_t closingBracket = content.find(']', openingBracket);
        bool isEmpty = true;
        for (size_t i = openingBracket + 1; i < closingBracket; ++i) {
            if (!isspace(content[i])) {
                isEmpty = false;
                break;
            }
        }

        if (!isEmpty) {
            content.insert(closingBracket, ",\r\n" + integrationJson);
        } else {
            content.insert(closingBracket, "\r\n" + integrationJson + "\r\n  ");
        }
    } else {
        if (integrationPos == std::string::npos) {
            return; 
        }
        size_t start = content.rfind('{', integrationPos);
        size_t end = content.find('}', integrationPos) + 1;
        size_t comma_before = content.rfind(',', start);
        if (comma_before != std::string::npos && content.substr(comma_before + 1, start - (comma_before + 1)).find_first_not_of(" \r\n\t") == std::string::npos) {
            content.erase(comma_before, end - comma_before);
        } else {
            content.erase(start, end - start);

            size_t integrationsArrayPos = content.find("\"CustomIntegrations\": [");
            size_t openingBracket = content.find('[', integrationsArrayPos);
            size_t closingBracket = content.find(']', openingBracket);
            bool isEmpty = true;
            for (size_t i = openingBracket + 1; i < closingBracket; ++i) {
                if (!isspace(content[i])) {
                    isEmpty = false; break;
                }
            }
            if (isEmpty) content.erase(openingBracket + 1, closingBracket - (openingBracket + 1));
        }
    }

    std::ofstream outFile(settingsPath, std::ios::binary);
    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
    } else {
        ShowTrayNotification(L"Bloxstrap Integration Error", L"Failed to write to Settings.json. Check permissions.");
        g_bloxstrapIntegration = false;
    }
}
// ==========

// FPS Capper
void FpsCapperThread()
{
    g_isFpsCapperRunning = true;

    std::vector<HANDLE> threadHandles;
    std::mutex handlesMutex;
    auto last_update = std::chrono::steady_clock::now();

    while (g_isFpsCapperRunning && g_fpsLimit > 0)
    {
        if (!g_isAfkStarted.load() || g_isFpsCapperPaused.load() || g_fpsLimit <= 0) {
            Sleep(250);
            continue;
        }
        long long frame_duration_ms = 1000LL / g_fpsLimit;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_update).count();

        if (elapsed >= 2) {
            last_update = now;

            std::vector<HANDLE> newThreadHandles;
            HWND foreground_wnd = g_unlockFpsOnFocus.load() ? GetForegroundWindow() : NULL;

            std::vector<DWORD> pidsToCap;
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snap != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32 pe = { 0 };
                pe.dwSize = sizeof(PROCESSENTRY32);
                if (Process32First(snap, &pe)) {
                    do {
                        if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0 || (g_fishstrapSupport.load() && _wcsicmp(pe.szExeFile, L"eurotrucks2.exe") == 0)) {
                            bool shouldCap = true;
                            if (foreground_wnd) {
                                auto procWins = GetWindowsForProcess(pe.th32ProcessID, true);
                                bool isFocused = std::any_of(procWins.begin(), procWins.end(), [foreground_wnd](HWND w) {
                                    return w == foreground_wnd;
                                });
                                if (isFocused) {
                                    shouldCap = false;
                                }
                            }
                            if (shouldCap) {
                                pidsToCap.push_back(pe.th32ProcessID);
                            }
                        }
                    } while (Process32Next(snap, &pe));
                }
                CloseHandle(snap);
            }

            for (DWORD pid : pidsToCap) {
                HANDLE thSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                if (thSnap != INVALID_HANDLE_VALUE) {
                    THREADENTRY32 te = { 0 };
                    te.dwSize = sizeof(THREADENTRY32);
                    if (Thread32First(thSnap, &te)) {
                        do {
                            if (te.th32OwnerProcessID == pid) {
                                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
                                if (hThread) {
                                    newThreadHandles.push_back(hThread);
                                }
                            }
                        } while (Thread32Next(thSnap, &te));
                    }
                    CloseHandle(thSnap);
                }
            }

            std::lock_guard<std::mutex> lock(handlesMutex);
            for (HANDLE h : threadHandles) {
                ResumeThread(h); 
                CloseHandle(h);
            }
            threadHandles = newThreadHandles;
        }

        std::lock_guard<std::mutex> lock(handlesMutex);
        if (threadHandles.empty()) {
            Sleep(500); 
        } else {
            for (HANDLE hThread : threadHandles) {
                SuspendThread(hThread);
            }

            long long sleep_duration = frame_duration_ms - 1;
            if (sleep_duration > 0) {
                Sleep(static_cast<DWORD>(sleep_duration));
            }

            for (HANDLE hThread : threadHandles) {
                ResumeThread(hThread);
            }

            Sleep(5);
        }
    }

    std::lock_guard<std::mutex> lock(handlesMutex);
    for (HANDLE h : threadHandles) {
        ResumeThread(h);
        CloseHandle(h);
    }
    threadHandles.clear();

    g_isFpsCapperRunning = false;
}
// ==========

// UserSafe 
void MonitorUserActivity()
{
    g_monitorThreadRunning = true;
    g_lastActivityTime = GetTickCount64();

    while (g_monitorThreadRunning)
    {
        int currentMode = g_userSafeMode;
        if (currentMode == 1)
        {
            bool activity = false;
            for (int i = 1; i < 256; i++)
            {
                if (GetAsyncKeyState(i) & 0x8000)
                {
                    activity = true;
                    break;
                }
            }

            if (activity)
            {
                g_lastActivityTime = GetTickCount64();
                g_userActive = true;
            }
            else if ((GetTickCount64() - g_lastActivityTime) / 1000 >= USER_INACTIVITY_WAIT)
            {
                g_userActive = false;
            }
        }
        else if (currentMode == 2)
        {
            LASTINPUTINFO lii;
            lii.cbSize = sizeof(LASTINPUTINFO);
            if (GetLastInputInfo(&lii))
            {
                if (lii.dwTime != (DWORD)g_lastActivityTime.load())
                {
                    g_lastActivityTime = lii.dwTime;
                    g_userActive = true;
                }
                else if ((GetTickCount() - lii.dwTime) / 1000 >= USER_INACTIVITY_WAIT)
                {
                    g_userActive = false;
                }
            }
        }

        Sleep(250);
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
// ==========

// Settings
void SaveSettings()
{
    HKEY hKey;
    DWORD multiSupport = g_multiSupport.load();
    DWORD fishstrapSupport = g_fishstrapSupport.load();
    DWORD autoUpdate = g_autoUpdate.load();
    DWORD autoStartAfk = g_autoStartAfk.load();
    DWORD autoReconnect = g_autoReconnect.load();
    DWORD restoreMethod = g_restoreMethod;
    DWORD tutorialShown = g_tutorialShown.load();
    DWORD bloxstrapIntegration = g_bloxstrapIntegration.load();
    DWORD unlockFpsOnFocus = g_unlockFpsOnFocus.load();
    uint64_t afkActions = g_afkActionsPerformed.load();
    uint64_t autoReconnects = g_autoReconnectsPerformed.load();
    DWORD useLegacyUi = g_useLegacyUi.load();
    DWORD fpsLimit = g_fpsLimit;


    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, L"MultiSupport", 0, REG_DWORD, (const BYTE*)&multiSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"FishstrapSupport", 0, REG_DWORD, (const BYTE*)&fishstrapSupport, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedTime", 0, REG_DWORD, (const BYTE*)&g_selectedTime, sizeof(DWORD));
        RegSetValueEx(hKey, L"SelectedAction", 0, REG_DWORD, (const BYTE*)&g_selectedAction, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoUpdate", 0, REG_DWORD, (const BYTE*)&autoUpdate, sizeof(DWORD));
        RegSetValueEx(hKey, L"UserSafeMode", 0, REG_DWORD, (const BYTE*)&g_userSafeMode, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoStartAfk", 0, REG_DWORD, (const BYTE*)&autoStartAfk, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoReconnect", 0, REG_DWORD, (const BYTE*)&autoReconnect, sizeof(DWORD));
        RegSetValueEx(hKey, L"RestoreMethod", 0, REG_DWORD, (const BYTE*)&restoreMethod, sizeof(DWORD));
        RegSetValueEx(hKey, L"TutorialShown", 0, REG_DWORD, (const BYTE*)&tutorialShown, sizeof(DWORD));
        RegSetValueEx(hKey, L"TotalAfkTime", 0, REG_QWORD, (const BYTE*)&g_totalAfkTimeSeconds, sizeof(uint64_t));
        RegSetValueEx(hKey, L"AfkActions", 0, REG_QWORD, (const BYTE*)&afkActions, sizeof(uint64_t));
        RegSetValueEx(hKey, L"AutoReconnects", 0, REG_QWORD, (const BYTE*)&autoReconnects, sizeof(uint64_t));

        RegSetValueEx(hKey, L"BloxstrapIntegration", 0, REG_DWORD, (const BYTE*)&bloxstrapIntegration, sizeof(DWORD));
        RegSetValueEx(hKey, L"UseLegacyUI", 0, REG_DWORD, (const BYTE*)&useLegacyUi, sizeof(DWORD));
        RegSetValueEx(hKey, L"FpsLimit", 0, REG_DWORD, (const BYTE*)&fpsLimit, sizeof(DWORD));
        RegSetValueEx(hKey, L"UnlockFpsOnFocus", 0, REG_DWORD, (const BYTE*)&unlockFpsOnFocus, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}
void LoadSettings()
{
    HKEY hKey;
   DWORD multiSupport = 0, fishstrapSupport = 0, selectedTime = 540, selectedAction = 0, autoStartAfk = 0;
    DWORD autoUpdate = 1, userSafeMode = 0, autoReconnect = 0, autoLimitRam = 0, restoreMethod = 1;
    DWORD tutorialShown = 0, useLegacyUi = 0, bloxstrapIntegration = 0, fpsLimit = 0, unlockFpsOnFocus = 0;
    uint64_t totalAfkTime = 0, afkActions = 0, autoReconnects = 0;
    DWORD dataSize = sizeof(DWORD);
    DWORD dataSize64 = sizeof(uint64_t);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey, L"MultiSupport", NULL, NULL, (LPBYTE)&multiSupport, &dataSize);
        RegQueryValueEx(hKey, L"FishstrapSupport", NULL, NULL, (LPBYTE)&fishstrapSupport, &dataSize);
        RegQueryValueEx(hKey, L"SelectedTime", NULL, NULL, (LPBYTE)&selectedTime, &dataSize);
        RegQueryValueEx(hKey, L"SelectedAction", NULL, NULL, (LPBYTE)&selectedAction, &dataSize);
        RegQueryValueEx(hKey, L"AutoUpdate", NULL, NULL, (LPBYTE)&autoUpdate, &dataSize);
        RegQueryValueEx(hKey, L"UserSafeMode", NULL, NULL, (LPBYTE)&userSafeMode, &dataSize);
        RegQueryValueEx(hKey, L"AutoStartAfk", NULL, NULL, (LPBYTE)&autoStartAfk, &dataSize);
        RegQueryValueEx(hKey, L"AutoReconnect", NULL, NULL, (LPBYTE)&autoReconnect, &dataSize);
        RegQueryValueEx(hKey, L"RestoreMethod", NULL, NULL, (LPBYTE)&restoreMethod, &dataSize);
        RegQueryValueEx(hKey, L"TutorialShown", NULL, NULL, (LPBYTE)&tutorialShown, &dataSize);
        RegQueryValueEx(hKey, L"TotalAfkTime", NULL, NULL, (LPBYTE)&totalAfkTime, &dataSize64);
        RegQueryValueEx(hKey, L"AfkActions", NULL, NULL, (LPBYTE)&afkActions, &dataSize64);
        RegQueryValueEx(hKey, L"AutoReconnects", NULL, NULL, (LPBYTE)&autoReconnects, &dataSize64);
        RegQueryValueEx(hKey, L"UseLegacyUI", NULL, NULL, (LPBYTE)&useLegacyUi, &dataSize);
        RegQueryValueEx(hKey, L"BloxstrapIntegration", NULL, NULL, (LPBYTE)&bloxstrapIntegration, &dataSize);
        RegQueryValueEx(hKey, L"FpsLimit", NULL, NULL, (LPBYTE)&fpsLimit, &dataSize);
        RegQueryValueEx(hKey, L"UnlockFpsOnFocus", NULL, NULL, (LPBYTE)&unlockFpsOnFocus, &dataSize);
        RegCloseKey(hKey);
    }

    g_multiSupport = multiSupport;
    g_fishstrapSupport = fishstrapSupport;
    g_selectedTime = selectedTime;
    g_selectedAction = selectedAction;
    g_autoUpdate = autoUpdate;
    g_userSafeMode = userSafeMode;
    g_autoStartAfk = autoStartAfk;
    g_autoReconnect = autoReconnect;
    g_restoreMethod = restoreMethod;
    g_tutorialShown = tutorialShown;
    g_totalAfkTimeSeconds = totalAfkTime;
    g_afkActionsPerformed = afkActions;
    g_autoReconnectsPerformed = autoReconnects;
    g_useLegacyUi = useLegacyUi;
    g_bloxstrapIntegration = bloxstrapIntegration;
    g_fpsLimit = fpsLimit;
    g_unlockFpsOnFocus = unlockFpsOnFocus;
}
void ResetSettings()
{
    g_multiSupport = false;
    g_fishstrapSupport = false;
    g_selectedTime = 540;
    g_selectedAction = 0;
    g_autoUpdate = true;
    g_userSafeMode = 0;
    g_autoStartAfk = false;
    g_autoReconnect = false;
    g_restoreMethod = 1;
    g_tutorialShown = false;
    g_totalAfkTimeSeconds = 0;
    g_afkActionsPerformed = 0;
    g_autoReconnectsPerformed = 0;
    g_manuallyStoppedPids.clear();
    g_useLegacyUi = false;
    g_bloxstrapIntegration = false;
    g_fpsLimit = 0;
    g_unlockFpsOnFocus = false;

    if (g_monitorThreadRunning.load())
    {
        StopActivityMonitor();
    }

    if (g_hMultiInstanceMutex)
    {
        DisableMultiInstanceSupport();
    }

    if (g_isFpsCapperRunning.load()) {
        g_isFpsCapperRunning = false;
        if (g_fpsCapperThread.joinable()) g_fpsCapperThread.join();
    }

    RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Agzes\\AntiAFK-RBX");
}
// ==========

// Tray menu
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

    if (!g_useLegacyUi.load())
    {
        wchar_t statusText[128];
        swprintf_s(statusText, L"Status: %s", afk ? L"Running" : L"Stopped");
        AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, statusText);
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING | (afk ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
        AppendMenu(g_hMenu, MF_STRING | (afk ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
        AppendMenu(g_hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING, ID_OPEN_UI, L"Open AntiAFK-RBX");
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING | (g_useLegacyUi.load() ? MF_CHECKED : 0), ID_USE_LEGACY_UI, L"Use Legacy UI (Tray)");
        if (g_updateFound)
            AppendMenu(g_hMenu, MF_STRING, ID_UPDATE_AVAILABLE, L"[⭳] • Update Available");
        AppendMenu(g_hMenu, MF_STRING, ID_EXIT, L"Exit");
        return;
    }

    wchar_t infoText[128];
    swprintf_s(infoText, L"Beta: %s | With ❤️", g_Version);
    AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, infoText);
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
    const wchar_t* actionNames[] = { L"Space (Jump)", L"W/S", L"Zoom (I/O)" };
    swprintf_s(actionLabel, L"Set Action • %s", actionNames[g_selectedAction]);
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hActionSubmenu, actionLabel);
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_SHOW_WINDOW, L"Show Roblox");
    AppendMenu(g_hMenu, MF_STRING, ID_HIDE_WINDOW, L"Hide Roblox");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    HMENU hSettingsSubmenu = CreatePopupMenu();
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_fishstrapSupport.load() ? MF_CHECKED : 0), ID_FISHSTRAP_SUP, L"FishStrap/Shaders Support");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoUpdate.load() ? MF_CHECKED : 0), ID_AUTO_UPDATE, L"Update Checker");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoStartAfk.load() ? MF_CHECKED : 0), ID_AUTO_START_AFK, L"Auto-Start AntiAFK");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoReconnect.load() ? MF_CHECKED : 0), ID_AUTO_RECONNECT, L"Auto Reconnect [new] [beta]");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_bloxstrapIntegration.load() ? MF_CHECKED : 0), ID_BLOXSTRAP_INTEGRATION, L"Bloxstrap/Fishstrap Integration");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_unlockFpsOnFocus.load() ? MF_CHECKED : 0), ID_UNLOCK_FPS_ON_FOCUS, L"Unlock FPS when focus");

    HMENU hUserSafeSubmenu = CreatePopupMenu();
    AppendMenu(hUserSafeSubmenu, MF_STRING | (g_userSafeMode == 0 ? MF_CHECKED : 0), ID_USER_SAFE_OFF, L"Off");
    AppendMenu(hUserSafeSubmenu, MF_STRING | (g_userSafeMode == 1 ? MF_CHECKED : 0), ID_USER_SAFE_LEGACY, L"Legacy (Keys/Clicks)");
    AppendMenu(hUserSafeSubmenu, MF_STRING | (g_userSafeMode == 2 ? MF_CHECKED : 0), ID_USER_SAFE_BETA, L"Beta (All Input)");
    const wchar_t* userSafeNames[] = { L"Off", L"Legacy", L"Beta" };
    wchar_t userSafeLabel[48];
    swprintf_s(userSafeLabel, L"User-Safe Mode • %s", userSafeNames[g_userSafeMode]);
    AppendMenu(hSettingsSubmenu, MF_POPUP | MF_STRING, (UINT_PTR)hUserSafeSubmenu, userSafeLabel);
    AppendMenu(hSettingsSubmenu, MF_SEPARATOR, 0, NULL);

    HMENU hRestoreSubmenu = CreatePopupMenu();
    AppendMenu(hRestoreSubmenu, MF_STRING | (g_restoreMethod == 0 ? MF_CHECKED : 0), ID_RESTORE_OFF, L"Off");
    AppendMenu(hRestoreSubmenu, MF_STRING | (g_restoreMethod == 1 ? MF_CHECKED : 0), ID_RESTORE_FOREGROUND, L"SetForeground");
    AppendMenu(hRestoreSubmenu, MF_STRING | (g_restoreMethod == 2 ? MF_CHECKED : 0), ID_RESTORE_ALTTAB, L"Alt+Tab [games]");
    const wchar_t* restoreNames[] = { L"Off", L"SetFor.", L"Alt+Tab" };
    wchar_t restoreLabel[48];
    swprintf_s(restoreLabel, L"Restore Window Method • %s", restoreNames[g_restoreMethod]);
    AppendMenu(hSettingsSubmenu, MF_POPUP | MF_STRING, (UINT_PTR)hRestoreSubmenu, restoreLabel);
    AppendMenu(hSettingsSubmenu, MF_SEPARATOR, 0, NULL);

    HMENU hFpsCapperSubmenu = CreatePopupMenu();
    AppendMenu(hFpsCapperSubmenu, MF_STRING, ID_FPS_CAP_CUSTOM, L"Custom FPS limit...");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 0 ? MF_CHECKED : 0), ID_FPS_CAP_OFF, L"Off");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 3 ? MF_CHECKED : 0), ID_FPS_CAP_3, L"3 FPS");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 5 ? MF_CHECKED : 0), ID_FPS_CAP_5, L"5 FPS");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 7 ? MF_CHECKED : 0), ID_FPS_CAP_7, L"7 FPS");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 10 ? MF_CHECKED : 0), ID_FPS_CAP_10, L"10 FPS");
    AppendMenu(hFpsCapperSubmenu, MF_STRING | (g_fpsLimit == 15 ? MF_CHECKED : 0), ID_FPS_CAP_15, L"15 FPS");
    wchar_t fpsLabel[48];
    if (g_fpsLimit == 0) swprintf_s(fpsLabel, L"FPS Capper* • Off");
    else swprintf_s(fpsLabel, L"FPS Capper* • %d FPS", g_fpsLimit);
    AppendMenu(hSettingsSubmenu, MF_POPUP | MF_STRING, (UINT_PTR)hFpsCapperSubmenu, fpsLabel);


    AppendMenu(hSettingsSubmenu, MF_STRING, ID_RESET_SETTINGS, L"Reset Settings");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSettingsSubmenu, L"Settings");
    AppendMenu(g_hMenu, MF_STRING | (g_useLegacyUi.load() ? MF_CHECKED : 0), ID_USE_LEGACY_UI, L"Use Legacy UI (Tray)");
    AppendMenu(g_hMenu, MF_STRING | (g_multiSupport.load() ? MF_CHECKED : 0), ID_MULTI_SUPPORT, L"Multi-Instance Support");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    if (g_updateFound)
        AppendMenu(g_hMenu, MF_STRING, ID_UPDATE_AVAILABLE, L"[⭳] • Update Available");
    AppendMenu(g_hMenu, MF_STRING, ID_EXIT, L"Exit");
}
void UpdateTrayIcon()
{
    g_nid.hIcon = CreateCustomIcon();
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}
// ==========

bool CheckForUpdates(bool showNotification = true)
{
    bool updateFound = false;
    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/2.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return false;
    HINTERNET hConnect = InternetOpenUrl(hInternet, L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/version", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[16] = { 0 };
    DWORD bytesRead = 0;
    if (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead))
    {
        buffer[bytesRead] = 0;
        try {
            int serverVersion = std::stoi(buffer);
            if (serverVersion > currentVersion) {
                updateFound = true;
                g_updateFound = true;
                if (showNotification) {
                    wchar_t msg[128];
                    swprintf_s(msg, L"New version available! Download on: https://github.com/Agzes/AntiAFK-RBX");
                    ShowTrayNotification(L"AntiAFK-RBX | Update Available", msg);
                }
                if (g_hwnd) {
                    CreateTrayMenu(g_isAfkStarted.load());
                }
            }
        }
        catch (const std::invalid_argument& ia) {
        }
        catch (const std::out_of_range& oor) {
        }
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return updateFound;
}

// Custom Interval Window
struct CustomIntervalData {
    HFONT hFontTitle = NULL, hFontText = NULL, hFontInput = NULL;
    RECT closeButtonRect = { 0 }, okButtonRect = { 0 }, cancelButtonRect = { 0 }, inputRect = { 0 };
    bool isHoveringClose = false, isHoveringOk = false, isHoveringCancel = false;
    bool isTrackingMouse = false;
    bool hasFocus = false; 
    wchar_t inputText[10] = { 0 };
    HCURSOR hCursorHand = NULL, hCursorArrow = NULL, hCursorText = NULL;
};
LRESULT CALLBACK CustomIntervalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CustomIntervalData* pData = (CustomIntervalData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new CustomIntervalData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        EnableAcrylic(hwnd);

        EnableAcrylic(hwnd);

        enum DWM_WINDOW_CORNER_PREFERENCE { DWMWCP_ROUND = 2 };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);
        pData->hFontTitle = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontInput = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int margin = 8, btnH = 30, btnGap = 8, inputH = 28;

        pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, 30 };

        int btnY = clientRect.bottom - margin - btnH;
        pData->inputRect = { margin, btnY - btnGap - inputH, clientRect.right - margin, btnY - btnGap };
        int totalWidth = clientRect.right - (margin * 2);
        int btnWidth = (totalWidth - btnGap) / 2;

        pData->cancelButtonRect = { margin, btnY, margin + btnWidth, btnY + btnH };
        pData->okButtonRect = { margin + btnWidth + btnGap, btnY, margin + btnWidth + btnGap + btnWidth, btnY + btnH };

        _itow_s(g_selectedTime, pData->inputText, 10);

        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);
        pData->hCursorText = LoadCursor(NULL, IDC_IBEAM);

        SetFocus(hwnd);
        return 0;
    }
    case WM_SETFOCUS:
    {
        pData->hasFocus = true;
        CreateCaret(hwnd, (HBITMAP)NULL, 1, pData->inputRect.bottom - pData->inputRect.top - 8);
        ShowCaret(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_KILLFOCUS:
    {
        pData->hasFocus = false;
        HideCaret(hwnd);
        DestroyCaret();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (!pData->hasFocus && PtInRect(&pData->inputRect, pt)) {
            SetFocus(hwnd);
        }

        if (PtInRect(&pData->okButtonRect, pt)) {
            int sec = _wtoi(pData->inputText);
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
        }
        else if (PtInRect(&pData->cancelButtonRect, pt) || PtInRect(&pData->closeButtonRect, pt)) {
            DestroyWindow(hwnd);
        }
        return 0;
    }
    case WM_CHAR:
    {
        if (wParam >= '0' && wParam <= '9') {
            size_t len = wcslen(pData->inputText);
            if (len < 6) {
                pData->inputText[len] = (wchar_t)wParam;
                pData->inputText[len + 1] = L'\0';
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        else if (wParam == VK_BACK) {
            size_t len = wcslen(pData->inputText);
            if (len > 0) {
                pData->inputText[len - 1] = L'\0';
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }


    case WM_APP + 10:
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            PostMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pData->okButtonRect.left, pData->okButtonRect.top));
            return 0;
        }
        else if (wParam == VK_ESCAPE)
        {
            PostMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pData->cancelButtonRect.left, pData->cancelButtonRect.top));
            return 0;
        }
        break;
    case WM_MOUSEMOVE:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool hover_ok = PtInRect(&pData->okButtonRect, pt);
        bool hover_cancel = PtInRect(&pData->cancelButtonRect, pt);
        bool hover_close = PtInRect(&pData->closeButtonRect, pt);
        bool hover_input = PtInRect(&pData->inputRect, pt);

        if (hover_ok != pData->isHoveringOk) { pData->isHoveringOk = hover_ok; InvalidateRect(hwnd, &pData->okButtonRect, FALSE); }
        if (hover_cancel != pData->isHoveringCancel) { pData->isHoveringCancel = hover_cancel; InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE); }
        if (hover_close != pData->isHoveringClose) { pData->isHoveringClose = hover_close; InvalidateRect(hwnd, &pData->closeButtonRect, FALSE); }

        if (hover_ok || hover_cancel || hover_close) SetCursor(pData->hCursorHand);
        else if (hover_input) SetCursor(pData->hCursorText);
        else SetCursor(pData->hCursorArrow);

        if ((hover_ok || hover_cancel || hover_close) && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringOk = pData->isHoveringCancel = pData->isHoveringClose = false;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, &pData->okButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        SetCursor(pData->hCursorArrow);
        return 0;
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            RECT dragRect = { 0, 0, pData->closeButtonRect.left, 30 };
            if (PtInRect(&dragRect, pt)) return HTCAPTION;
        }
        return hit;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        Graphics g(memDC);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        SolidBrush bgBrush(Color(200, 35, 35, 35)); 
        g.FillRectangle(&bgBrush, (REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)clientRect.bottom);

        HFONT oldFont = (HFONT)SelectObject(memDC, pData->hFontTitle);
        Font fontTitle(memDC, pData->hFontTitle);
        SolidBrush textBrush_Initial(Color(255, 220, 220, 220));
        g.DrawString(L"Set Custom Interval", -1, &fontTitle, PointF(8.0f, 6.0f), &textBrush_Initial);

        Font fontText(memDC, pData->hFontText);
        SolidBrush grayBrush(Color(255, 180, 180, 180));
        g.DrawString(L"Enter interval in seconds (max 1200):", -1, &fontText, PointF(8.0f, 30.0f), &grayBrush);

        SolidBrush inputBrush(Color(255, 50, 50, 50)); 
        Pen inputPen(pData->hasFocus ? Color(255, 0, 122, 204) : Color(255, 100, 100, 100), 1.0f);
        
        GraphicsPath inputPath;
        inputPath.AddArc((REAL)pData->inputRect.left, (REAL)pData->inputRect.top, 5.0f * 2, 5.0f * 2, 180, 90);
        inputPath.AddArc((REAL)pData->inputRect.right - 5.0f * 2, (REAL)pData->inputRect.top, 5.0f * 2, 5.0f * 2, 270, 90);
        inputPath.AddArc((REAL)pData->inputRect.right - 5.0f * 2, (REAL)pData->inputRect.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 0, 90);
        inputPath.AddArc((REAL)pData->inputRect.left, (REAL)pData->inputRect.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 90, 90);
        inputPath.CloseFigure();

        g.FillPath(&inputBrush, &inputPath);
        g.DrawPath(&inputPen, &inputPath);

        Font inputFont(memDC, pData->hFontInput);
        SolidBrush inputTextBrush(Color(255, 255, 255, 255));
        RECT textRect = pData->inputRect;
        InflateRect(&textRect, -8, 0);

        StringFormat sf;
        sf.SetAlignment(StringAlignmentNear);
        sf.SetLineAlignment(StringAlignmentCenter);
        RectF textRectF((REAL)textRect.left, (REAL)textRect.top, (REAL)(textRect.right - textRect.left), (REAL)(textRect.bottom - textRect.top));
        g.DrawString(pData->inputText, -1, &inputFont, textRectF, &sf, &inputTextBrush);

        if (pData->hasFocus) {
             RectF size, layout((REAL)0, (REAL)0, (REAL)1000, (REAL)1000);
             g.MeasureString(pData->inputText, -1, &inputFont, layout, &size);
             
             int caretX = pData->inputRect.left + 8 + (int)size.Width;
             int caretY = pData->inputRect.top + 5;
             SetCaretPos(caretX, caretY);
        }

        auto drawBtn = [&](const RECT& r, const wchar_t* txt, bool hover, bool primary) {
            Color fill = primary ? (hover ? Color(255, 20, 142, 224) : Color(255, 0, 122, 204)) : (hover ? Color(255, 80, 80, 80) : Color(255, 60, 60, 60));
            SolidBrush br(fill);
            GraphicsPath path;
            path.AddArc((REAL)r.left, (REAL)r.top, 5.0f * 2, 5.0f * 2, 180, 90);
            path.AddArc((REAL)r.right - 5.0f * 2, (REAL)r.top, 5.0f * 2, 5.0f * 2, 270, 90);
            path.AddArc((REAL)r.right - 5.0f * 2, (REAL)r.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 0, 90);
            path.AddArc((REAL)r.left, (REAL)r.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 90, 90);
            path.CloseFigure();
            g.FillPath(&br, &path);

            Font font(memDC, pData->hFontText);
            SolidBrush textBrush(Color(255, 255, 255, 255));
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            RectF rectF((REAL)r.left, (REAL)r.top, (REAL)(r.right - r.left), (REAL)(r.bottom - r.top));
            g.DrawString(txt, -1, &font, rectF, &sf, &textBrush);
            };
        drawBtn(pData->okButtonRect, L"OK", pData->isHoveringOk, true);
        drawBtn(pData->cancelButtonRect, L"Cancel", pData->isHoveringCancel, false);

        DrawSharedCloseButton(&g, pData->closeButtonRect, pData->isHoveringClose);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (pData) {
            if (pData->hFontTitle) DeleteObject(pData->hFontTitle);
            if (pData->hFontText) DeleteObject(pData->hFontText);
            if (pData->hFontInput) DeleteObject(pData->hFontInput);
            if (pData->hCursorHand) DestroyCursor(pData->hCursorHand);
            if (pData->hCursorArrow) DestroyCursor(pData->hCursorArrow);
            if (pData->hCursorText) DestroyCursor(pData->hCursorText);
            delete pData;
            HWND owner = GetWindow(hwnd, GW_OWNER);
            if (owner)
            {
                EnableWindow(owner, TRUE);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
            g_hCustomIntervalWnd = NULL;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
void ShowCustomIntervalDialog(HWND owner)
{
    if (g_hCustomIntervalWnd && IsWindow(g_hCustomIntervalWnd))
    {
        SetForegroundWindow(g_hCustomIntervalWnd);
        return;
    }

    static bool registered = false;
    if (!registered)
    {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = CustomIntervalWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = L"AntiAFK-RBX-CustomInterval"; 
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
        registered = RegisterClass(&wc) != 0;
    }

    const int dlgW = 300, dlgH = 122;
    RECT ownerRect = { 0 };
    if (owner) GetWindowRect(owner, &ownerRect);
    int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dlgW) / 2;
    int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dlgH) / 2;


    g_hCustomIntervalWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, L"AntiAFK-RBX-CustomInterval",
        L"Set Custom Interval", WS_POPUP,
        x, y, dlgW, dlgH, owner, NULL, g_hInst, NULL);
    if (g_hCustomIntervalWnd)
    {
        SetLayeredWindowAttributes(g_hCustomIntervalWnd, 0, 255, LWA_ALPHA);
        if (owner) EnableWindow(owner, FALSE); 
        ShowWindow(g_hCustomIntervalWnd, SW_SHOW);
        UpdateWindow(g_hCustomIntervalWnd);
    }
}
// ==========

// Custom FPS Window
struct CustomFpsData {
    HFONT hFontTitle = NULL, hFontText = NULL, hFontInput = NULL;
    RECT closeButtonRect = { 0 }, okButtonRect = { 0 }, cancelButtonRect = { 0 }, inputRect = { 0 };
    bool isHoveringClose = false, isHoveringOk = false, isHoveringCancel = false;
    bool isTrackingMouse = false;
    bool hasFocus = false;
    wchar_t inputText[10] = { 0 };
    HCURSOR hCursorHand = NULL, hCursorArrow = NULL, hCursorText = NULL;
};
LRESULT CALLBACK CustomFpsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CustomFpsData* pData = (CustomFpsData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new CustomFpsData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        EnableAcrylic(hwnd);

        enum DWM_WINDOW_CORNER_PREFERENCE { DWMWCP_ROUND = 2 };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);
        pData->hFontTitle = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontInput = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int margin = 8, btnH = 30, btnGap = 8, inputH = 28;

        pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, 30 };

        int btnY = clientRect.bottom - margin - btnH;
        pData->inputRect = { margin, btnY - btnGap - inputH, clientRect.right - margin, btnY - btnGap };
        int totalWidth = clientRect.right - (margin * 2);
        int btnWidth = (totalWidth - btnGap) / 2;

        pData->cancelButtonRect = { margin, btnY, margin + btnWidth, btnY + btnH };
        pData->okButtonRect = { margin + btnWidth + btnGap, btnY, margin + btnWidth + btnGap + btnWidth, btnY + btnH };

        _itow_s(g_fpsLimit, pData->inputText, 10);

        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);
        pData->hCursorText = LoadCursor(NULL, IDC_IBEAM);

        SetFocus(hwnd);
        return 0;
    }
    case WM_SETFOCUS:
    {
        pData->hasFocus = true;
        CreateCaret(hwnd, (HBITMAP)NULL, 1, pData->inputRect.bottom - pData->inputRect.top - 8);
        ShowCaret(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_KILLFOCUS:
    {
        pData->hasFocus = false;
        HideCaret(hwnd);
        DestroyCaret();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (!pData->hasFocus && PtInRect(&pData->inputRect, pt)) {
            SetFocus(hwnd);
        }

        if (PtInRect(&pData->okButtonRect, pt)) {
            int fps = _wtoi(pData->inputText);
            if (fps < 0) fps = 0;
            if (fps > 60) fps = 60; // limit to 60
            PostMessage(g_hwnd, WM_COMMAND, ID_FPS_CAP_CUSTOM_BASE + fps, 0);
            DestroyWindow(hwnd);
        }
        else if (PtInRect(&pData->cancelButtonRect, pt) || PtInRect(&pData->closeButtonRect, pt)) {
            DestroyWindow(hwnd);
        }
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool hover_ok = PtInRect(&pData->okButtonRect, pt);
        bool hover_cancel = PtInRect(&pData->cancelButtonRect, pt);
        bool hover_close = PtInRect(&pData->closeButtonRect, pt);
        bool hover_input = PtInRect(&pData->inputRect, pt);

        if (hover_ok != pData->isHoveringOk) { pData->isHoveringOk = hover_ok; InvalidateRect(hwnd, &pData->okButtonRect, FALSE); }
        if (hover_cancel != pData->isHoveringCancel) { pData->isHoveringCancel = hover_cancel; InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE); }
        if (hover_close != pData->isHoveringClose) { pData->isHoveringClose = hover_close; InvalidateRect(hwnd, &pData->closeButtonRect, FALSE); }

        if (hover_ok || hover_cancel || hover_close) SetCursor(pData->hCursorHand);
        else if (hover_input) SetCursor(pData->hCursorText);
        else SetCursor(pData->hCursorArrow);

        if ((hover_ok || hover_cancel || hover_close) && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringOk = pData->isHoveringCancel = pData->isHoveringClose = false;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, &pData->okButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        SetCursor(pData->hCursorArrow);
        return 0;
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            RECT dragRect = { 0, 0, pData->closeButtonRect.left, 30 };
            if (PtInRect(&dragRect, pt)) return HTCAPTION;
        }
        return hit;
    }
    case WM_CHAR:
    {
        if (wParam >= '0' && wParam <= '9') {
            size_t len = wcslen(pData->inputText);
            if (len < 3) {
                pData->inputText[len] = (wchar_t)wParam;
                pData->inputText[len + 1] = L'\0';
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        else if (wParam == VK_BACK) {
            size_t len = wcslen(pData->inputText);
            if (len > 0) {
                pData->inputText[len - 1] = L'\0';
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == VK_RETURN)
        {
            PostMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pData->okButtonRect.left + 1, pData->okButtonRect.top + 1));
            return 0;
        }
        else if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        Graphics g(memDC);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        SolidBrush bgBrush(Color(240, 35, 35, 35));
        g.FillRectangle(&bgBrush, (REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)clientRect.bottom);

        HFONT oldFont = (HFONT)SelectObject(memDC, pData->hFontTitle);
        Font fontTitle(memDC, pData->hFontTitle);
        SolidBrush textBrush_Initial(Color(255, 220, 220, 220));
        g.DrawString(L"Set Custom FPS Limit", -1, &fontTitle, PointF(8.0f, 6.0f), &textBrush_Initial);

        Font fontText(memDC, pData->hFontText);
        SolidBrush grayBrush(Color(255, 180, 180, 180));
        g.DrawString(L"Enter FPS limit (0=off, max 60):", -1, &fontText, PointF(8.0f, 30.0f), &grayBrush);

        SolidBrush inputBrush(Color(255, 50, 50, 50));
        Pen inputPen(pData->hasFocus ? Color(255, 0, 122, 204) : Color(255, 100, 100, 100), 1.0f);
        
        GraphicsPath inputPath;
        inputPath.AddArc((REAL)pData->inputRect.left, (REAL)pData->inputRect.top, 5.0f * 2, 5.0f * 2, 180, 90);
        inputPath.AddArc((REAL)pData->inputRect.right - 5.0f * 2, (REAL)pData->inputRect.top, 5.0f * 2, 5.0f * 2, 270, 90);
        inputPath.AddArc((REAL)pData->inputRect.right - 5.0f * 2, (REAL)pData->inputRect.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 0, 90);
        inputPath.AddArc((REAL)pData->inputRect.left, (REAL)pData->inputRect.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 90, 90);
        inputPath.CloseFigure();

        g.FillPath(&inputBrush, &inputPath);
        g.DrawPath(&inputPen, &inputPath);

        Font inputFont(memDC, pData->hFontInput);
        SolidBrush inputBrush2(Color(255, 255, 255, 255));
        
        StringFormat sf;
        sf.SetAlignment(StringAlignmentNear);
        sf.SetLineAlignment(StringAlignmentCenter);
        
        RECT rect;
        rect.top = pData->inputRect.top;
        rect.bottom = pData->inputRect.bottom;
        rect.left = pData->inputRect.left;
        rect.right = pData->inputRect.right;
        
        InflateRect(&rect, -8, 0);
        
        RectF textRectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
        g.DrawString(pData->inputText, -1, &inputFont, textRectF, &sf, &inputBrush2);

        if (pData->hasFocus) {
            RectF size, layout((REAL)0, (REAL)0, (REAL)1000, (REAL)1000);
            g.MeasureString(pData->inputText, -1, &inputFont, layout, &size);
            
            int caretX = pData->inputRect.left + 8 + (int)size.Width;
            int caretY = pData->inputRect.top + 5;
            SetCaretPos(caretX, caretY);
        }

        auto drawBtn = [&](const RECT& r, const wchar_t* txt, bool hover, bool primary) {
            Color fill = primary ? (hover ? Color(255, 20, 142, 224) : Color(255, 0, 122, 204)) : (hover ? Color(255, 80, 80, 80) : Color(255, 60, 60, 60));
            SolidBrush br(fill);
            GraphicsPath path;
            path.AddArc((REAL)r.left, (REAL)r.top, 10.0f, 10.0f, 180, 90);
            path.AddArc((REAL)r.right - 10.0f, (REAL)r.top, 10.0f, 10.0f, 270, 90);
            path.AddArc((REAL)r.right - 10.0f, (REAL)r.bottom - 10.0f, 10.0f, 10.0f, 0, 90);
            path.AddArc((REAL)r.left, (REAL)r.bottom - 10.0f, 10.0f, 10.0f, 90, 90);
            path.CloseFigure();
            g.FillPath(&br, &path);

            Font font(memDC, pData->hFontText);
            SolidBrush textBrush(Color(255, 255, 255, 255));
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            RectF rectF((REAL)r.left, (REAL)r.top, (REAL)(r.right - r.left), (REAL)(r.bottom - r.top));
            g.DrawString(txt, -1, &font, rectF, &sf, &textBrush);
            };
        drawBtn(pData->okButtonRect, L"OK", pData->isHoveringOk, true);
        drawBtn(pData->cancelButtonRect, L"Cancel", pData->isHoveringCancel, false);

        DrawSharedCloseButton(&g, pData->closeButtonRect, pData->isHoveringClose);

        SelectObject(memDC, oldFont);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (pData) {
            if (pData->hFontTitle) DeleteObject(pData->hFontTitle);
            if (pData->hFontText) DeleteObject(pData->hFontText);
            if (pData->hFontInput) DeleteObject(pData->hFontInput);
            if (pData->hCursorHand) DestroyCursor(pData->hCursorHand);
            if (pData->hCursorArrow) DestroyCursor(pData->hCursorArrow);
            if (pData->hCursorText) DestroyCursor(pData->hCursorText);
            delete pData;
            HWND owner = GetWindow(hwnd, GW_OWNER); 
            if (owner) {
                EnableWindow(owner, TRUE);
                SetForegroundWindow(owner);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
            g_hCustomFpsWnd = NULL;
        }
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
void ShowCustomFpsDialog(HWND owner)
{
    if (g_hCustomFpsWnd && IsWindow(g_hCustomFpsWnd))
    {
        SetForegroundWindow(g_hCustomFpsWnd);
        return;
    }

    static bool registered = false;
    const wchar_t CUSTOM_FPS_CLASS_NAME[] = L"AntiAFK-RBX-CustomFps";
    if (!registered)
    {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = CustomFpsWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = CUSTOM_FPS_CLASS_NAME;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
        registered = RegisterClass(&wc) != 0;
    }

    const int dlgW = 300, dlgH = 122;
    RECT ownerRect = { 0 };
    if (owner) GetWindowRect(owner, &ownerRect);
    int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dlgW) / 2;
    int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dlgH) / 2;

    g_hCustomFpsWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, CUSTOM_FPS_CLASS_NAME,
        L"Set Custom FPS", WS_POPUP,
        x, y, dlgW, dlgH, owner, NULL, g_hInst, NULL);
    if (g_hCustomFpsWnd)
    {
        SetLayeredWindowAttributes(g_hCustomFpsWnd, 0, 255, LWA_ALPHA);
        if (owner) EnableWindow(owner, FALSE);
        ShowWindow(g_hCustomFpsWnd, SW_SHOW);
        UpdateWindow(g_hCustomFpsWnd);
    }
}
// ==========

// Tutorial Window
struct TutorialData {
    HFONT hFont40 = NULL, hFont20 = NULL, hFont14 = NULL, hFont13 = NULL, hFont11 = NULL, hFont14b = NULL;
    RECT nextButtonRect = { 0 }, backButtonRect = { 0 }, noButtonRect = { 0 };
    RECT skipLinkRect = { 0 }, githubLinkRect = { 0 }, wikiLinkRect = { 0 }, closeButtonRect = { 0 };
    bool isHoveringNext = false, isHoveringBack = false, isHoveringNo = false;
    bool isHoveringSkip = false, isHoveringGithub = false, isHoveringWiki = false, isHoveringClose = false;
    bool isTrackingMouse = false;
    int page = 0; // 0: welcome, 1: htu, 2: htw, 3: tips, 4: usersafe, 5: final
    float progress = 0.0f;
    UINT_PTR uTimerId = 0;
    HCURSOR hCursorHand = NULL, hCursorArrow = NULL;
};
LRESULT CALLBACK TutorialWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static TutorialData* pData = nullptr;

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new TutorialData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        EnableAcrylic(hwnd);

        enum DWM_WINDOW_CORNER_PREFERENCE { DWMWCP_ROUND = 2 };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        pData->hFont40 = CreateFontW(-MulDiv(28, dpiY, 72), 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI Black");
        pData->hFont20 = CreateFontW(-MulDiv(14, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont14 = CreateFontW(-MulDiv(14, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont14b = CreateFontW(-MulDiv(13, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont13 = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFont11 = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        const int dlgW = 472, dlgH = 312;
        const int margin = 10;
        const int btnW = 131, btnH = 32, progressBarHeight = 4;
        int y_pos = dlgH - margin - btnH - progressBarHeight;

        pData->nextButtonRect = { dlgW - margin - btnW, y_pos, dlgW - margin, y_pos + btnH };
        pData->backButtonRect = { margin, y_pos, margin + btnW, y_pos + btnH };
        pData->skipLinkRect = { dlgW - margin - btnW - 10 - 48, y_pos + 4, dlgW - margin - btnW - 10, y_pos + 4 + 24 };
        pData->githubLinkRect = { pData->backButtonRect.right, y_pos, pData->nextButtonRect.left, y_pos + btnH };
        pData->wikiLinkRect = { margin, 220, margin + 250, 220 + 20 };
        pData->closeButtonRect = { dlgW - 46, 0, dlgW, 30 };

        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);

        int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - dlgW) / 2, y = (screenH - dlgH) / 2;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, dlgW, dlgH, SWP_SHOWWINDOW);

        pData->uTimerId = timeSetEvent(16, 1, [](UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
            if (IsWindow((HWND)dwUser)) {
                InvalidateRect((HWND)dwUser, NULL, FALSE);
            }
            }, (DWORD_PTR)hwnd, TIME_PERIODIC);
        return 0;
    }
    case WM_NCHITTEST:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);

        if (PtInRect(&pData->closeButtonRect, pt)) return HTCLIENT;

        RECT dragRect = { 0, 0, pData->closeButtonRect.left, 30 };
        if (PtInRect(&dragRect, pt)) return HTCAPTION;

        return HTCLIENT;
    }
    case WM_MOUSEMOVE:
    {
        if (!pData) break;
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        auto checkHover = [&](bool& hoverFlag, const RECT& rect) {
            bool nowHovering = PtInRect(&rect, pt);
            if (nowHovering != hoverFlag) {
                hoverFlag = nowHovering;
                InvalidateRect(hwnd, &rect, FALSE);
            }
            return nowHovering;
        };

        bool anyHover = false;
        anyHover |= checkHover(pData->isHoveringClose, pData->closeButtonRect);
        anyHover |= checkHover(pData->isHoveringNext, pData->nextButtonRect);
        if (pData->page > 0) anyHover |= checkHover(pData->isHoveringBack, pData->backButtonRect);
        if (pData->page == 0) anyHover |= checkHover(pData->isHoveringSkip, pData->skipLinkRect);
        if (pData->page == 2) anyHover |= checkHover(pData->isHoveringGithub, pData->githubLinkRect);
        if (pData->page == 4) anyHover |= checkHover(pData->isHoveringWiki, pData->wikiLinkRect);
        if (pData->page == 3 || pData->page == 4) anyHover |= checkHover(pData->isHoveringNo, pData->noButtonRect);

        SetCursor(anyHover ? pData->hCursorHand : pData->hCursorArrow);

        if (anyHover && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        if (!pData) break;
        pData->isHoveringNext = pData->isHoveringBack = pData->isHoveringNo = false;
        pData->isHoveringSkip = pData->isHoveringGithub = pData->isHoveringWiki = pData->isHoveringClose = false;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, NULL, FALSE);
        SetCursor(pData->hCursorArrow);
        return 0;
    case WM_LBUTTONDOWN:
    {
        if (!pData) break;
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (PtInRect(&pData->closeButtonRect, pt)) { PostMessage(hwnd, WM_CLOSE, 0, 0); return 0; }
        if (pData->page == 0 && PtInRect(&pData->skipLinkRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_BTN_SKIP, 0); return 0; }
        if (PtInRect(&pData->nextButtonRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_BTN_NEXT, 0); return 0; }
        if (pData->page > 0 && PtInRect(&pData->backButtonRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_BTN_BACK, 0); return 0; }
        if ((pData->page == 3 || pData->page == 4) && PtInRect(&pData->noButtonRect, pt)) { PostMessage(hwnd, WM_COMMAND, IDNO, 0); return 0; }
        if (pData->page == 2 && PtInRect(&pData->githubLinkRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_LINKGITHUB, 0); return 0; }
        if (pData->page == 4 && PtInRect(&pData->wikiLinkRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_LINKWIKI_TUTORIAL, 0); return 0; }

        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (!pData) break;
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt)) {
            ShowWindow(hwnd, SW_MINIMIZE);
            return 0;
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_LINKGITHUB:
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            break;
        case ID_LINKWIKI_TUTORIAL:
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX/wiki/User-Safe-Mode", NULL, NULL, SW_SHOWNORMAL);
            break;
        case ID_BTN_SKIP:
            g_tutorialShown = true;
            SaveSettings();
            PostMessage(g_hwnd, WM_COMMAND, ID_OPEN_UI, 0);
            DestroyWindow(hwnd);
            break;
        case ID_BTN_NEXT:
            if (pData->page == 5)
            {
                g_tutorialShown = true;
                PostMessage(g_hwnd, WM_COMMAND, ID_OPEN_UI, 0);
                SaveSettings();
                CreateTrayMenu(g_isAfkStarted.load());
                DestroyWindow(hwnd);
            }
            else
            {
                if (pData->page == 3)
                {
                    g_autoStartAfk = true;
                }
                if (pData->page == 4)
                {
                    g_userSafeMode = 2;
                    if (!g_monitorThreadRunning.load())
                        StartActivityMonitor();
                }
                pData->page++;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case ID_BTN_BACK:
            if (pData->page > 0)
            {
                pData->page--;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case IDNO:
            if (pData->page == 3 || pData->page == 4)
            {
                if (pData->page == 3)
                {
                    g_autoStartAfk = false;
                }
                if (pData->page == 4)
                {
                    g_userSafeMode = 0;
                    if (g_monitorThreadRunning.load())
                        StopActivityMonitor();
                }
                pData->page++;
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
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        FillRect(memDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetBkMode(memDC, TRANSPARENT);

        const int dlgW = clientRect.right;
        const int dlgH = clientRect.bottom;

        const wchar_t* nextText = L"Next";
        if (pData->page == 3 || pData->page == 4) nextText = L"Yes";
        else if (pData->page == 5) nextText = L"Got it!";

        if (pData->page == 3 || pData->page == 4) {
            const int margin = 10, btnW = 131, btnH = 32, gap = 10, progressBarHeight = 4;
            int y_pos = dlgH - margin - btnH - progressBarHeight;
            int x_next = dlgW - margin - btnW;
            int x_no = x_next - btnW - gap;
            pData->noButtonRect = { x_no, y_pos, x_no + btnW, y_pos + btnH }; 
        }

        if (pData->page == 0)
        {
            const int margin = 20;
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush grayBrush(Color(255, 180, 180, 180));
            int y_pos = 60;
            g.DrawString(L"Welcome to", -1, &font20, PointF((REAL)margin + 3, (REAL)y_pos + 2), &grayBrush);
            
            Font font40(memDC, pData->hFont40);
            const wchar_t* p1 = L"AntiAFK-";
            const wchar_t* p2 = L"RBX";
            
            RectF s1;
            g.MeasureString(p1, -1, &font40, PointF(0,0), &s1);

            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            SolidBrush redBrush(Color(255, 226, 35, 26));

            g.DrawString(p1, -1, &font40, PointF((REAL)margin, (REAL)y_pos + 15), &whiteBrush);
            g.DrawString(p2, -1, &font40, PointF((REAL)margin + s1.Width, (REAL)y_pos + 15), &redBrush);
            
            Font font13(memDC, pData->hFont13);
            SolidBrush textBrush(Color(255, 200, 200, 200));
            RectF rSub((REAL)margin, (REAL)y_pos + 15 + s1.Height + 10, (REAL)clientRect.right - margin, (REAL)clientRect.bottom - 80);
            g.DrawString(L"This quick tutorial will walk you through the key features and help with the initial setup.", -1, &font13, rSub, NULL, &textBrush);
        }
        else if (pData->page == 1)
        {
            const int margin = 20;
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            g.DrawString(L"How to Use?", -1, &font20, PointF((REAL)margin, 20), &whiteBrush);

            Font font13(memDC, pData->hFont13);
            RectF rText((REAL)margin, (REAL)55 + 5, (REAL)clientRect.right - margin, (REAL)80);
            const wchar_t* howto = L"AntiAFK-RBX is a tray application with UI. You can open it from the tray menu. The tray icon (next to the clock) shows the current status and provides quick access.";
            g.DrawString(howto, -1, &font13, rText, NULL, &whiteBrush);

            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            sfCenter.SetLineAlignment(StringAlignmentCenter);
            
            RectF rPrevLabel(0, rText.GetBottom() + 15, (REAL)clientRect.right, 20);
            g.DrawString(L"The tray icon is dynamic and shows the current status:", -1, &font13, rPrevLabel, &sfCenter, &whiteBrush);

            int iconsY = (int)rPrevLabel.GetBottom() + 8;
            int iconSize = 32;
            int gap = 90;
            int totalW = iconSize * 3 + gap * 2;
            int startX = (clientRect.right - clientRect.left - totalW) / 2;
            HICON icoOnSingle = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ON), IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
            HICON icoOff = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_OFF), IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
            HICON icoOnMulti = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ON_MULTI), IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
            DrawIconEx(memDC, startX + 0 * (iconSize + gap), iconsY, icoOnSingle, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DrawIconEx(memDC, startX + 1 * (iconSize + gap), iconsY, icoOff, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DrawIconEx(memDC, startX + 2 * (iconSize + gap), iconsY, icoOnMulti, iconSize, iconSize, 0, NULL, DI_NORMAL);
            DestroyIcon(icoOff);
            DestroyIcon(icoOnSingle);
            DestroyIcon(icoOnMulti);

            sfCenter.SetLineAlignment(StringAlignmentNear);
            
            int labelWidth = iconSize + gap;
            int labelY = iconsY + iconSize + 4;
            RectF rL1((REAL)startX - (gap - iconSize) / 2, (REAL)labelY, (REAL)iconSize + gap, 24);
            RectF rL2((REAL)rL1.GetRight(), (REAL)labelY, (REAL)iconSize + gap, 24);
            RectF rL3((REAL)rL2.GetRight(), (REAL)labelY, (REAL)iconSize + gap, 24);
            
            g.DrawString(L"ON (Single)", -1, &font13, rL1, &sfCenter, &whiteBrush);
            g.DrawString(L"OFF", -1, &font13, rL2, &sfCenter, &whiteBrush);
            g.DrawString(L"ON (Multi)", -1, &font13, rL3, &sfCenter, &whiteBrush);
        }
        else if (pData->page == 2)
        {
            const int margin = 20;
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            g.DrawString(L"How it Works?", -1, &font20, PointF((REAL)margin, 20), &whiteBrush);
            
            Font font13(memDC, pData->hFont13);
            RectF rText((REAL)margin, (REAL)20 + 55, (REAL)clientRect.right - margin, (REAL)clientRect.bottom - 80);
            const wchar_t* workText = L"When the program starts, it starts another thread (AntiAFK thread), which checks whether the roblox window is running, and then performs actions on the user's settings, if the settings change during the process, the thread is restarted.";
            g.DrawString(workText, -1, &font13, rText, NULL, &whiteBrush);
        }
        else if (pData->page == 3)
        {
            const int margin = 20;
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            g.DrawString(L"Tips To Use", -1, &font20, PointF((REAL)margin, 20), &whiteBrush);

            Font font14b(memDC, pData->hFont14b);
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            RectF rTextTitle((REAL)margin, (REAL)20 + 55 + 25, (REAL)clientRect.right - margin, (REAL)clientRect.bottom - 120);
            g.DrawString(L"Enable “Auto-Start”?", -1, &font14b, rTextTitle, &sfCenter, &whiteBrush);
            
            Font font13(memDC, pData->hFont13);
            RectF rTextBody((REAL)margin, (REAL)rTextTitle.Y + 30, (REAL)clientRect.right - margin, (REAL)100);
            const wchar_t* tipsText = L"This feature will automatically start and stop the Anti-AFK function when you open or close Roblox. It's highly recommended for convenience.";
            g.DrawString(tipsText, -1, &font13, rTextBody, &sfCenter, &whiteBrush);

            Font font11(memDC, pData->hFont11);
            SolidBrush grayBrush(Color(255, 123, 123, 123));
            RectF rSubText((REAL)margin, (REAL)pData->nextButtonRect.top - 30, (REAL)clientRect.right - margin, (REAL)20);
            g.DrawString(L"You can change this later in the 'Automation' tab.", -1, &font11, rSubText, &sfCenter, &grayBrush);
        }
        else if (pData->page == 4)
        {
            const int margin = 20;
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            g.DrawString(L"Tips To Use", -1, &font20, PointF((REAL)margin, 20), &whiteBrush);

            Font font14b(memDC, pData->hFont14b);
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            RectF rTextTitle((REAL)margin, (REAL)20 + 55 + 25, (REAL)clientRect.right - margin, (REAL)clientRect.bottom - 120);
            g.DrawString(L"Enable “User-Safe Mode”?", -1, &font14b, rTextTitle, &sfCenter, &whiteBrush);
            
            Font font13(memDC, pData->hFont13);
            RectF rTextBody((REAL)margin, (REAL)rTextTitle.Y + 30, (REAL)clientRect.right - margin, (REAL)100);
            const wchar_t* tipsText = L"This mode prevents the Anti-AFK action from interrupting you if you are actively using your computer. It will wait for you to be idle. Recommended.";
            g.DrawString(tipsText, -1, &font13, rTextBody, &sfCenter, &whiteBrush);

            pData->wikiLinkRect = { 0, (int)rTextBody.Y + 65, clientRect.right, (int)rTextBody.Y + 85 };
            pData->wikiLinkRect.left = (clientRect.right - 100) / 2;
            pData->wikiLinkRect.right = pData->wikiLinkRect.left + 100;

            Font font11(memDC, pData->hFont11);
            SolidBrush grayBrush(Color(255, 123, 123, 123));
            RectF rSubText((REAL)margin, (REAL)pData->nextButtonRect.top - 30, (REAL)clientRect.right - margin, (REAL)20);
            g.DrawString(L"You can change this later in the 'General' tab.", -1, &font11, rSubText, &sfCenter, &grayBrush);
        }
        else if (pData->page == 5)
        {
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            
            RectF rTop((REAL)0, (REAL)60, (REAL)clientRect.right, (REAL)30);
            g.DrawString(L"Thank for using", -1, &font20, rTop, &sfCenter, &whiteBrush);

            Font font40(memDC, pData->hFont40);
            const wchar_t* p1 = L"AntiAFK-";
            const wchar_t* p2 = L"RBX";
            
            RectF s1;
            g.MeasureString(p1, -1, &font40, PointF(0,0), &s1);
            RectF s2;
            g.MeasureString(p2, -1, &font40, PointF(0,0), &s2);
            
            REAL totalW = s1.Width + s2.Width;
            REAL startX = ((REAL)clientRect.right - totalW) / 2;
            
            SolidBrush redBrush(Color(255, 226, 35, 26));

            g.DrawString(p1, -1, &font40, PointF(startX, (REAL)rTop.GetBottom() - 5), &whiteBrush);
            g.DrawString(p2, -1, &font40, PointF(startX + s1.Width, (REAL)rTop.GetBottom() - 5), &redBrush);

            Font font13(memDC, pData->hFont13);
            SolidBrush grayBrush(Color(255, 200, 200, 200));
            RectF rBottom((REAL)20, (REAL)rTop.GetBottom() + s1.Height + 25, (REAL)clientRect.right - 20, (REAL)clientRect.bottom - 80);
            g.DrawString(L"You're all set! Remember, you can change these settings anytime in the main window. This tutorial can also be reopened from the tray menu.", -1, &font13, rBottom, &sfCenter, &grayBrush);
        }

        auto drawBtn = [&](const RECT& r, const wchar_t* txt, bool hover, bool primary) {
            COLORREF fill = primary ? (hover ? RGB(20, 142, 224) : RGB(0, 122, 204)) : (hover ? RGB(70, 70, 70) : RGB(55, 55, 55));
            HBRUSH br = CreateSolidBrush(fill);
            HPEN pen = (HPEN)GetStockObject(NULL_PEN);
            SelectObject(memDC, br); SelectObject(memDC, pen);
            RoundRect(memDC, r.left, r.top, r.right, r.bottom, 5, 5);
            DeleteObject(br);
            SetTextColor(memDC, DARK_TEXT);
            SelectObject(memDC, pData->hFont13);
            DrawTextW(memDC, txt, -1, (LPRECT)&r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        };

        auto drawLink = [&](const RECT& r, const wchar_t* txt, bool hover) {
            SetTextColor(memDC, hover ? RGB(180, 180, 180) : RGB(0x77, 0x77, 0x77));
            SelectObject(memDC, pData->hFont11);
            UINT format = (pData->page == 4 || pData->page == 2) ? (DT_CENTER | DT_VCENTER | DT_SINGLELINE) : (DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            DrawTextW(memDC, txt, -1, (LPRECT)&r, format);
        };

        drawBtn(pData->nextButtonRect, nextText, pData->isHoveringNext, true);
        if (pData->page > 0) drawBtn(pData->backButtonRect, L"Back", pData->isHoveringBack, false);
        if (pData->page == 3 || pData->page == 4) drawBtn(pData->noButtonRect, L"No", pData->isHoveringNo, false);
        if (pData->page == 0) drawLink(pData->skipLinkRect, L"Skip", pData->isHoveringSkip);
        if (pData->page == 2) drawLink(pData->githubLinkRect, L"See more on GitHub", pData->isHoveringGithub);
        if (pData->page == 4) drawLink(pData->wikiLinkRect, L"More in Wiki", pData->isHoveringWiki);

        const int progressBarHeight = 4;
        RECT progressBarBgRect = { 0, clientRect.bottom - progressBarHeight, clientRect.right, clientRect.bottom };
        HBRUSH bgBrush = CreateSolidBrush(RGB(45, 45, 45));
        FillRect(memDC, &progressBarBgRect, bgBrush);
        DeleteObject(bgBrush);

        float targetProgress = (float)(pData->page + 1) / 6.0f;
        if (pData->progress < targetProgress) {
            pData->progress += 0.03f;
            if (pData->progress > targetProgress) pData->progress = targetProgress;
        }
        else if (pData->progress > targetProgress) {
            pData->progress -= 0.03f;
            if (pData->progress < targetProgress) pData->progress = targetProgress;
        }

        RECT progressBarFgRect = { 0, clientRect.bottom - progressBarHeight, (int)(clientRect.right * pData->progress), clientRect.bottom };
        HBRUSH fgBrush = CreateSolidBrush(RGB(0, 122, 204));
        FillRect(memDC, &progressBarFgRect, fgBrush);
        DeleteObject(fgBrush);

        About_Paint_DrawCloseButton(memDC, pData->closeButtonRect, pData->isHoveringClose);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (pData) {
            if (pData->uTimerId != 0) {
                timeKillEvent(pData->uTimerId);
            }
            if (pData->hFont40) DeleteObject(pData->hFont40);
            if (pData->hFont20) DeleteObject(pData->hFont20);
            if (pData->hFont14) DeleteObject(pData->hFont14);
            if (pData->hFont14b) DeleteObject(pData->hFont14b);
            if (pData->hFont13) DeleteObject(pData->hFont13);
            if (pData->hFont11) DeleteObject(pData->hFont11);
            if (pData->hCursorHand) DestroyCursor(pData->hCursorHand);
            if (pData->hCursorArrow) DestroyCursor(pData->hCursorArrow);
            delete pData;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
            HWND owner = GetWindow(hwnd, GW_OWNER);
            if (owner) {
                EnableWindow(owner, TRUE);
            }
            g_hCustomIntervalWnd = NULL;
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
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = TutorialWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = L"AntiAFK-RBX-Tutorial";
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }
    HWND h = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, L"AntiAFK-RBX-Tutorial", L"AntiAFK-RBX | Tutorial", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 472, 312, owner, NULL, g_hInst, NULL);
    if (h) { ShowWindow(h, SW_SHOW); UpdateWindow(h); }
}
// ==========

// Main UI Window
LRESULT CALLBACK MainUIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowMainUIDialog(HWND owner)
{
    if (g_hMainUiWnd && IsWindow(g_hMainUiWnd))
    {
        SetForegroundWindow(g_hMainUiWnd);
        return;
    }

    static bool registered = false;
    const wchar_t MAINUI_CLASS_NAME[] = L"AntiAFK-RBX-MainUI";
    if (!registered)
    {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = MainUIWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = MAINUI_CLASS_NAME;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 380, winH = 460;
    int x = (screenW - winW) / 2, y = (screenH - winH) / 2;

    g_hMainUiWnd = CreateWindowEx(WS_EX_APPWINDOW, MAINUI_CLASS_NAME,
        L"AntiAFK-RBX Settings", WS_POPUP,
        x, y, winW, winH, owner, NULL, g_hInst, NULL);

    if (g_hMainUiWnd)
    {
        enum DWM_WINDOW_CORNER_PREFERENCE {
            DWMWCP_DEFAULT = 0,
            DWMWCP_DONOTROUND = 1,
            DWMWCP_ROUND = 2,
            DWMWCP_ROUNDSMALL = 3
        };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_hMainUiWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
        EnableAcrylic(g_hMainUiWnd);
        ShowWindow(g_hMainUiWnd, SW_SHOW);
        UpdateWindow(g_hMainUiWnd);
    }
}
struct MainUIData {
    HFONT hFontTitle = NULL, hFontSection = NULL, hFontText = NULL, hFontSmall = NULL, hFontNav = NULL, hFontNavSelected = NULL;
    RECT closeButtonRect = { 0 };
    RECT iconButtonRect = { 0 };
    RECT startButtonRect = { 0 };

    int currentPage = 0; // 0: General, 1: Automation, 2: Stats, 3: Advanced
    std::vector<std::pair<std::wstring, RECT>> navItems;
    int hoveringNavItem = -1;
    float navIndicatorX = 0.0f;
    float navIndicatorWidth = 0.0f;

    RECT intervalDropdownRect = { 0 };
    RECT actionDropdownRect = { 0 };
    RECT multiInstanceToggleRect = { 0 };
    RECT fishstrapToggleRect = { 0 };
    RECT autoUpdateToggleRect = { 0 };
    RECT autoStartToggleRect = { 0 };
    RECT autoReconnectToggleRect = { 0 };
    RECT bloxstrapIntegrationToggleRect = { 0 };
    RECT userSafeDropdownRect = { 0 };
    RECT resetStatsButtonRect = { 0 };
    RECT unlockFpsOnFocusToggleRect = { 0 };
    RECT restoreMethodDropdownRect = { 0 };
    RECT legacyUiToggleRect = { 0 };
    std::vector<RECT> helpButtonRects;
    RECT fpsCapperDropdownRect = { 0 };
    RECT resetSettingsButtonRect = { 0 };

    POINT hoverPoint = { 0 };
    bool isHoveringClose = false;
    bool isHoveringIcon = false;
    bool isHoveringStart = false;
    float startButtonScale = 1.0f;
    float startButtonVelocity = 0.0f;
    bool isPressingStart = false;
    bool isHoveringReset = false;
    bool isHoveringResetStats = false;
    bool isHoveringInterval = false;
    bool isHoveringAction = false;
    bool isHoveringUserSafe = false;
    bool isHoveringRestore = false;
    bool isHoveringFpsCapper = false;
    int hoveringHelpButton = -1;

    std::wstring startButtonErrorText;
    uint64_t startButtonErrorTime = 0;
    float errorAnimationProgress = 0.0f; 
    int errorAnimationDirection = 0;

    UINT_PTR uTimerId = 0;
    bool isTrackingMouse = false;
    HCURSOR hCursorHand = NULL, hCursorArrow = NULL;
    HICON hIcon = NULL;

    float multiInstanceAnim = 0.0f;
    float fishstrapAnim = 0.0f;
    float autoUpdateAnim = 0.0f;
    float autoStartAnim = 0.0f;
    float autoReconnectAnim = 0.0f;
    float bloxstrapIntegrationAnim = 0.0f;
    float unlockFpsOnFocusAnim = 0.0f;
    float legacyUiAnim = 0.0f;

    bool previousAfkStartedState = false;
    float stateChangeProgress = 0.0f;
    int stateChangeDirection = 0;
};
void DrawRoundedRectangle(Graphics* g, Pen* pen, REAL x, REAL y, REAL width, REAL height, REAL radius)
{
    GraphicsPath path;
    path.AddArc(x, y, radius * 2, radius * 2, 180, 90);
    path.AddArc(x + width - (radius * 2), y, radius * 2, radius * 2, 270, 90);
    path.AddArc(x + width - (radius * 2), y + height - (radius * 2), radius * 2, radius * 2, 0, 90);
    path.AddArc(x, y + height - (radius * 2), radius * 2, radius * 2, 90, 90);
    path.CloseFigure();
    g->DrawPath(pen, &path);
}
void FillRoundedRectangle(Graphics* g, Brush* brush, REAL x, REAL y, REAL width, REAL height, REAL radius)
{
    GraphicsPath path;
    path.AddArc(x, y, radius * 2, radius * 2, 180, 90);
    path.AddArc(x + width - (radius * 2), y, radius * 2, radius * 2, 270, 90);
    path.AddArc(x + width - (radius * 2), y + height - (radius * 2), radius * 2, radius * 2, 0, 90);
    path.AddArc(x, y + height - (radius * 2), radius * 2, radius * 2, 90, 90);
    path.CloseFigure();
    g->FillPath(brush, &path);
}
void MainUI_Paint_DrawCloseButton(HDC hdc, const RECT& closeButtonRect, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    int x_center = (closeButtonRect.left + closeButtonRect.right) / 2;
    int y_center = (closeButtonRect.top + closeButtonRect.bottom) / 2;
    int cross_size = 5;

    DrawSharedCloseButton(&g, closeButtonRect, isHovering);
}
void MainUI_Paint_DrawToggle(HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool checked, bool isHovering, float animState) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF textRectF((REAL)20, (REAL)rect.top, (REAL)(rect.left - 10 - 20), (REAL)(rect.bottom - rect.top));
    g.DrawString(text, -1, &gdiFont, textRectF, &sf, &textBrush);

    int w = 50, h = 24;
    RECT toggleBackRect = { rect.left, rect.top + (rect.bottom - rect.top - h) / 2, rect.left + w, rect.top + (rect.bottom - rect.top - h) / 2 + h };

    Color baseColor = checked ? Color(255, 0, 122, 204) : Color(255, 80, 80, 80);
    Color hoverColor(baseColor.GetA(), 
                     static_cast<BYTE>(baseColor.GetR() * 0.85f), 
                     static_cast<BYTE>(baseColor.GetG() * 0.85f), 
                     static_cast<BYTE>(baseColor.GetB() * 0.85f));

    SolidBrush backBrush(isHovering ? hoverColor : baseColor);
    GraphicsPath path;
    path.AddArc(toggleBackRect.left, toggleBackRect.top, h, h, 90, 180);
    path.AddArc(toggleBackRect.right - h, toggleBackRect.top, h, h, 270, 180);
    path.CloseFigure();
    g.FillPath(&backBrush, &path);

    SolidBrush knobBrush(Color(255, 255, 255, 255));
    int knobSize = h - 6;
    int startX = toggleBackRect.left + 3;
    int endX = toggleBackRect.right - knobSize - 3;
    int knobX = startX + (int)((endX - startX) * animState);
    int knobY = toggleBackRect.top + 3;
    g.FillEllipse(&knobBrush, knobX, knobY, knobSize, knobSize);
}
void MainUI_Paint_DrawDropdown(HDC hdc, const RECT& rect, HFONT font, const wchar_t* label, const wchar_t* value, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    
    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    
    RectF labelRectF((REAL)20, (REAL)rect.top, (REAL)(rect.left - 15 - 20), (REAL)(rect.bottom - rect.top));
    g.DrawString(label, -1, &gdiFont, labelRectF, &sf, &textBrush);

    SolidBrush backBrush(isHovering ? Color(255, 65, 65, 65) : Color(255, 45, 45, 45));
    Pen borderPen(Color(255, 80, 80, 80));
    
    FillRoundedRectangle(&g, &backBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top), 5);
    DrawRoundedRectangle(&g, &borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top), 5);

    RECT valueRect = rect;
    InflateRect(&valueRect, -8, 0);
    RectF valueRectF((REAL)valueRect.left, (REAL)valueRect.top, (REAL)(valueRect.right - valueRect.left), (REAL)(valueRect.bottom - valueRect.top));
    g.DrawString(value, -1, &gdiFont, valueRectF, &sf, &textBrush);

    Pen arrowPen(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)), 2.0f);
    int arrowX = rect.right - 15;
    int arrowY = rect.top + (rect.bottom - rect.top) / 2;
    
    g.DrawLine(&arrowPen, (REAL)(arrowX - 4), (REAL)(arrowY - 2), (REAL)arrowX, (REAL)(arrowY + 2));
    g.DrawLine(&arrowPen, (REAL)arrowX, (REAL)(arrowY + 2), (REAL)(arrowX + 4), (REAL)(arrowY - 2));
}
void MainUI_Paint_DrawHelpButton(HDC hdc, const RECT& rect, HFONT font, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush backBrush(isHovering ? Color(255, 80, 80, 80) : Color(255, 55, 55, 55));
    Pen borderPen(Color(255, 90, 90, 90));

    g.FillEllipse(&backBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawEllipse(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF textRectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawString(L"?", -1, &gdiFont, textRectF, &sf, &textBrush);
}
void MainUI_HandleClick(HWND hwnd, POINT pt, MainUIData* pData) {
    for (int i = 0; i < pData->navItems.size(); ++i) {
        if (PtInRect(&pData->navItems[i].second, pt)) {
            pData->currentPage = i;
            PostMessage(hwnd, WM_SIZE, 0, 0); 
            return;
        }
    }

    if (PtInRect(&pData->iconButtonRect, pt)) {
        ShowAboutDialog(hwnd);
        return;
    }

    bool isErrorActive = !pData->startButtonErrorText.empty() && (GetTickCount64() - pData->startButtonErrorTime < 3000);
    if (PtInRect(&pData->startButtonRect, pt) && !isErrorActive) {
        pData->isPressingStart = true;
        PostMessage(g_hwnd, WM_COMMAND, g_isAfkStarted.load() ? ID_STOP_AFK : ID_START_AFK, 0);
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    if (pData->currentPage == 2 && PtInRect(&pData->resetStatsButtonRect, pt)) {
        PostMessage(g_hwnd, WM_APP + 12, 0, 0); 
        return;
    }

    for (int i = 0; i < pData->helpButtonRects.size(); ++i) {
        if (PtInRect(&pData->helpButtonRects[i], pt)) {
            const wchar_t* title = L"Help";
            const wchar_t* text = L"No help text available for this setting.";
            if (pData->currentPage == 0) { // General
                if (i == 0) { title = L"Interval"; text = L"How often to perform an Anti-AFK action. \nNote: Roblox kicks you after 20 minutes of inactivity."; }
                if (i == 1) { title = L"Action"; text = L"The keypress to simulate to prevent being kicked."; }
                if (i == 2) { title = L"Multi-Instance Support"; text = L"Allows running multiple Roblox clients at once. The Anti-AFK action will apply to all of them.\n\nHow to use:\n1. Close all Roblox clients\n2. Open AntiAFK-RBX\n3. Enable Multi-Instance Support\n4. Open Roblox clients"; }
                if (i == 3) { title = L"User-Safe Mode"; text = L"Pauses the Anti-AFK action if you are actively using your computer to avoid interruptions. It waits for you to be idle before proceeding.\n\n'Legacy': Checks for mouse clicks and key presses.\n'Beta': Checks for any system-wide input, which is more reliable."; }
                if (i == 4) { title = L"Restore Window"; text = L"How to return focus to your previous window after the action. \n\n'SetForeground' is recommended.\n'Alt+Tab' may work better for fullscreen games."; }
            } else if (pData->currentPage == 1) { // Automation
                if (i == 0) { title = L"Auto-Start AntiAFK"; text = L"Automatically starts and stops the Anti-AFK function when Roblox is opened or closed."; }
                if (i == 1) { title = L"Auto Reconnect"; text = L"Experimental: Automatically tries to Reconnect if you are kicked for being idle."; }
                if (i == 2) { title = L"Bloxstrap/Fishstrap Integration"; text = L"Automatically starts AntiAFK-RBX when launching Roblox via Bloxstrap or Fishstrap, but does not start AntiAFK."; }
            } else if (pData->currentPage == 3) { // Advanced
                if (i == 0) { title = L"FishStrap/Shaders Support"; text = L"Enables compatibility with modified clients (e.g., FishStrap) that use a different process name."; }
                if (i == 1) { title = L"Update Checker"; text = L"Automatically checks for new versions on startup and notifies you if an update is available."; }
                if (i == 2) { title = L"Use Legacy UI (Tray)"; text = L"Switches to the classic right-click tray menu. To return to new UI turn off \"Use Legacy UI (tray)\" in tray settings."; }
                if (i == 3) { title = L"FPS Capper*"; text = L"Experimental & Beta: Limits Roblox's FPS to reduce CPU/GPU usage. \n\nWarning: This feature works by limiting the process's refresh rate, not the FPS directly. Therefore, the actual in-game FPS might fluctuate slightly (or not) from the set limit. "; }
                if (i == 4) { title = L"Unlock FPS when focus"; text = L"Temporarily disables the FPS Capper* for the Roblox window you are currently playing on."; }
            }
            ShowDarkMessageBox(hwnd, text, title, MB_OK | MB_ICONINFORMATION);
            return;
        }
    }

    if (pData->currentPage == 0 && PtInRect(&pData->multiInstanceToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_MULTI_SUPPORT, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoStartToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_START_AFK, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoReconnectToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_RECONNECT, 0); return; }

    if (pData->currentPage == 3 && PtInRect(&pData->unlockFpsOnFocusToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UNLOCK_FPS_ON_FOCUS, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->bloxstrapIntegrationToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_BLOXSTRAP_INTEGRATION, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->fishstrapToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_FISHSTRAP_SUP, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->autoUpdateToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_UPDATE, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->legacyUiToggleRect, pt)) {
        int result = ShowDarkMessageBox(hwnd, L"Are you sure you want to switch to the legacy tray UI?\n\nThe modern UI window will be closed. To open it again, you will need to disable 'Use Legacy UI' from the tray menu and restart the application.", L"Switch to Legacy UI?", MB_YESNO | MB_ICONQUESTION);
        if (result == IDYES) {
            PostMessage(g_hwnd, WM_COMMAND, ID_USE_LEGACY_UI, 0);
            DestroyWindow(hwnd);
        }
        return;
    }


    if (pData->currentPage == 0) { 
        if (PtInRect(&pData->intervalDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, ID_TIME_CUSTOM, L"Custom interval...");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 3 * 60 ? MF_CHECKED : 0), ID_TIME_3, L"3 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 6 * 60 ? MF_CHECKED : 0), ID_TIME_6, L"6 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 9 * 60 ? MF_CHECKED : 0), ID_TIME_9, L"9 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 11 * 60 ? MF_CHECKED : 0), ID_TIME_11, L"11 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 13 * 60 ? MF_CHECKED : 0), ID_TIME_13, L"13 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 15 * 60 ? MF_CHECKED : 0), ID_TIME_15, L"15 minutes");
            AppendMenu(hMenu, MF_STRING | (g_selectedTime == 18 * 60 ? MF_CHECKED : 0), ID_TIME_18, L"18 minutes");
            POINT menuPt;
            menuPt.x = pData->intervalDropdownRect.left;
            menuPt.y = pData->intervalDropdownRect.bottom;
            ClientToScreen(hwnd, &menuPt);
            TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, menuPt.x, menuPt.y, 0, g_hwnd, NULL);
            DestroyMenu(hMenu);
            return;
        }
        if (PtInRect(&pData->actionDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | (g_selectedAction == 0 ? MF_CHECKED : 0), ID_ACTION_SPACE, L"Space (Jump)");
            AppendMenu(hMenu, MF_STRING | (g_selectedAction == 1 ? MF_CHECKED : 0), ID_ACTION_WS, L"W/S");
            AppendMenu(hMenu, MF_STRING | (g_selectedAction == 2 ? MF_CHECKED : 0), ID_ACTION_ZOOM, L"Zoom (I/O)");
            POINT menuPt;
            menuPt.x = pData->actionDropdownRect.left;
            menuPt.y = pData->actionDropdownRect.bottom;
            ClientToScreen(hwnd, &menuPt);
            TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, menuPt.x, menuPt.y, 0, g_hwnd, NULL);
            DestroyMenu(hMenu);
            return;
        }
        if (PtInRect(&pData->userSafeDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | (g_userSafeMode == 0 ? MF_CHECKED : 0), ID_USER_SAFE_OFF, L"Off");
            AppendMenu(hMenu, MF_STRING | (g_userSafeMode == 1 ? MF_CHECKED : 0), ID_USER_SAFE_LEGACY, L"Legacy (Keys/Clicks)");
            AppendMenu(hMenu, MF_STRING | (g_userSafeMode == 2 ? MF_CHECKED : 0), ID_USER_SAFE_BETA, L"Beta (All Input)");
            POINT menuPt;
            menuPt.x = pData->userSafeDropdownRect.left;
            menuPt.y = pData->userSafeDropdownRect.bottom;
            ClientToScreen(hwnd, &menuPt);
            TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, menuPt.x, menuPt.y, 0, g_hwnd, NULL);
            DestroyMenu(hMenu);
            return;
        }
        if (PtInRect(&pData->restoreMethodDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | (g_restoreMethod == 0 ? MF_CHECKED : 0), ID_RESTORE_OFF, L"Off");
            AppendMenu(hMenu, MF_STRING | (g_restoreMethod == 1 ? MF_CHECKED : 0), ID_RESTORE_FOREGROUND, L"SetForeground");
            AppendMenu(hMenu, MF_STRING | (g_restoreMethod == 2 ? MF_CHECKED : 0), ID_RESTORE_ALTTAB, L"Alt+Tab [games]");
            POINT menuPt;
            menuPt.x = pData->restoreMethodDropdownRect.left;
            menuPt.y = pData->restoreMethodDropdownRect.bottom;
            ClientToScreen(hwnd, &menuPt);
            TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, menuPt.x, menuPt.y, 0, g_hwnd, NULL);
            DestroyMenu(hMenu);
            return;
        }
    }

    if (pData->currentPage == 3) {
        if (PtInRect(&pData->fpsCapperDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, ID_FPS_CAP_CUSTOM, L"Custom FPS limit...");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 0 ? MF_CHECKED : 0), ID_FPS_CAP_OFF, L"Off");
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 3 ? MF_CHECKED : 0), ID_FPS_CAP_3, L"3 FPS");
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 5 ? MF_CHECKED : 0), ID_FPS_CAP_5, L"5 FPS");
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 7 ? MF_CHECKED : 0), ID_FPS_CAP_7, L"7 FPS");
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 10 ? MF_CHECKED : 0), ID_FPS_CAP_10, L"10 FPS");
            AppendMenu(hMenu, MF_STRING | (g_fpsLimit == 15 ? MF_CHECKED : 0), ID_FPS_CAP_15, L"15 FPS");
            POINT menuPt;
            menuPt.x = pData->fpsCapperDropdownRect.left;
            menuPt.y = pData->fpsCapperDropdownRect.bottom;
            ClientToScreen(hwnd, &menuPt);
            TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, menuPt.x, menuPt.y, 0, g_hwnd, NULL);
            DestroyMenu(hMenu);
            InvalidateRect(hwnd, NULL, FALSE); 
            return;
        }
    }

    if (pData->currentPage == 3 && PtInRect(&pData->resetSettingsButtonRect, pt)) {
        PostMessage(g_hwnd, WM_COMMAND, ID_RESET_SETTINGS, 0);
        return;
    }
}
void MainUI_Paint_DrawContent(HDC hdc, const RECT& clientRect, MainUIData* pData) 
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

    RECT topBarRect = { 0, 0, clientRect.right, 30 };
    Pen headerLinePen(Color(40, 255, 255, 255));
    g.DrawLine(&headerLinePen, 0, 30, clientRect.right, 30);

    SolidBrush mainBgBrush(Color(100, 10, 10, 10));
    g.FillRectangle(&mainBgBrush, (REAL)clientRect.left, (REAL)clientRect.top, (REAL)(clientRect.right - clientRect.left), (REAL)(clientRect.bottom - clientRect.top));

    if (pData->isHoveringIcon) {
        SolidBrush hoverBrush(Color(255, 70, 70, 70));
        g.FillRectangle(&hoverBrush, (REAL)pData->iconButtonRect.left, (REAL)pData->iconButtonRect.top, (REAL)(pData->iconButtonRect.right - pData->iconButtonRect.left), (REAL)(pData->iconButtonRect.bottom - pData->iconButtonRect.top));
    }
    if (pData->hIcon) {
        DrawIconEx(hdc, pData->iconButtonRect.left + (pData->iconButtonRect.right - pData->iconButtonRect.left - 16) / 2, (pData->iconButtonRect.bottom - 16) / 2, pData->hIcon, 16, 16, 0, NULL, DI_NORMAL);
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, pData->hFontTitle);
    SetTextColor(hdc, DARK_TEXT);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF titleRect((REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)30);
    Font gdiFont(hdc, pData->hFontTitle);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    g.DrawString(L"AntiAFK-RBX", -1, &gdiFont, titleRect, &sf, &textBrush);

    SelectObject(hdc, oldFont);

    bool isStarted = g_isAfkStarted.load();
    const wchar_t* startText = isStarted ? L"Stop" : L"Start";
    const wchar_t* prevText = pData->previousAfkStartedState ? L"Stop" : L"Start";

    COLORREF startColor = isStarted ? RGB(200, 30, 30) : RGB(0, 122, 204);
    COLORREF startHoverColor = isStarted ? RGB(230, 40, 40) : RGB(20, 142, 224);
    COLORREF currentStartColor = pData->isHoveringStart ? startHoverColor : startColor;

    COLORREF startColor_previous = pData->previousAfkStartedState ? RGB(200, 30, 30) : RGB(0, 122, 204);

    COLORREF baseColor = pData->isHoveringStart ? startHoverColor : currentStartColor;
    float stateProgress = pData->stateChangeProgress;
    BYTE r_state = (BYTE)(GetRValue(startColor_previous) * (1.0f - stateProgress) + GetRValue(baseColor) * stateProgress);
    BYTE g_state = (BYTE)(GetGValue(startColor_previous) * (1.0f - stateProgress) + GetGValue(baseColor) * stateProgress);
    BYTE b_state = (BYTE)(GetBValue(startColor_previous) * (1.0f - stateProgress) + GetBValue(baseColor) * stateProgress);

    COLORREF errorColor = RGB(80, 80, 80);
    float progress = pData->errorAnimationProgress;
    BYTE r = (BYTE)(r_state * (1.0f - progress) + GetRValue(errorColor) * progress);
    BYTE gr = (BYTE)(g_state * (1.0f - progress) + GetGValue(errorColor) * progress);
    BYTE b = (BYTE)(b_state * (1.0f - progress) + GetBValue(errorColor) * progress);
    SolidBrush startBrush(Color(255, r, gr, b));

    RECT baseRect = pData->startButtonRect;
    float scale = pData->startButtonScale;
    int newWidth = (int)((baseRect.right - baseRect.left) * scale);
    int newHeight = (int)((baseRect.bottom - baseRect.top) * scale);
    int centerX = baseRect.left + (baseRect.right - baseRect.left) / 2;
    int centerY = baseRect.top + (baseRect.bottom - baseRect.top) / 2;

    RECT scaledRect = { centerX - newWidth / 2, centerY - newHeight / 2, centerX + newWidth / 2, centerY + newHeight / 2 };

    FillRoundedRectangle(&g, &startBrush, (REAL)scaledRect.left, (REAL)scaledRect.top, (REAL)(scaledRect.right - scaledRect.left), (REAL)(scaledRect.bottom - scaledRect.top), 8.0f * scale);
    oldFont = (HFONT)SelectObject(hdc, pData->hFontSection);

    RectF textRectF((REAL)scaledRect.left, (REAL)scaledRect.top, (REAL)(scaledRect.right - scaledRect.left), (REAL)(scaledRect.bottom - scaledRect.top));
    Font gdiSectionFont(hdc, pData->hFontSection);

   if (stateProgress < 1.0f) {
        SolidBrush prevTextBrush(Color((BYTE)(255 * (1.0f - stateProgress) * (1.0f - progress)), 255, 255, 255));
        g.DrawString(prevText, -1, &gdiSectionFont, textRectF, &sf, &prevTextBrush);
    }
    if (stateProgress > 0.0f) {
        SolidBrush currentTextBrush(Color((BYTE)(255 * stateProgress * (1.0f - progress)), 255, 255, 255));
        g.DrawString(startText, -1, &gdiSectionFont, textRectF, &sf, &currentTextBrush);
    }

    if (progress > 0.0f && !pData->startButtonErrorText.empty()) {
        SolidBrush errorTextBrush(Color((BYTE)(255 * progress), 255, 255, 255));
        g.DrawString(pData->startButtonErrorText.c_str(), -1, &gdiSectionFont, textRectF, &sf, &errorTextBrush);
    }

    RECT navBarRect = { 0, 30, clientRect.right, 60 };
    SolidBrush navBrush(Color(100, 35, 35, 35));
    g.FillRectangle(&navBrush, (REAL)navBarRect.left, (REAL)navBarRect.top, (REAL)(navBarRect.right - navBarRect.left), (REAL)(navBarRect.bottom - navBarRect.top));

    for (size_t i = 0; i < pData->navItems.size(); ++i) {
        bool isSelected = (pData->currentPage == i);
        bool isHovering = (pData->hoveringNavItem == i);

        HFONT fontToUse = pData->hFontNav;
        Color textColor(255, 160, 160, 160);

        if (isSelected) {
            fontToUse = pData->hFontNavSelected;
            textColor = Color(255, 255, 255, 255);
        } else if (isHovering) {
            textColor = Color(255, 230, 230, 230);
        }
        
        Font navFont(hdc, fontToUse);
        SolidBrush navBrush(textColor);
        RectF navItemRect((REAL)pData->navItems[i].second.left, (REAL)pData->navItems[i].second.top, (REAL)(pData->navItems[i].second.right - pData->navItems[i].second.left), (REAL)(pData->navItems[i].second.bottom - pData->navItems[i].second.top));
        g.DrawString(pData->navItems[i].first.c_str(), -1, &navFont, navItemRect, &sf, &navBrush);
    }

    SolidBrush underlineBrush(Color(255, 0, 122, 204));
    RECT navUnderlineRect = { (int)pData->navIndicatorX, navBarRect.bottom - 2, (int)(pData->navIndicatorX + pData->navIndicatorWidth), navBarRect.bottom };
    g.FillRectangle(&underlineBrush, (REAL)navUnderlineRect.left, (REAL)navUnderlineRect.top, (REAL)(navUnderlineRect.right - navUnderlineRect.left), (REAL)(navUnderlineRect.bottom - navUnderlineRect.top));

    const int content_y_start = pData->startButtonRect.bottom + 20;
    const int content_x_start = 20;
    if (pData->currentPage == 0) { // General

        wchar_t buffer[64];
        swprintf_s(buffer, L"%d sec (%d min)", g_selectedTime, g_selectedTime / 60);
        MainUI_Paint_DrawDropdown(hdc, pData->intervalDropdownRect, pData->hFontText, L"Interval", buffer, pData->isHoveringInterval);

        const wchar_t* actionNames[] = { L"Space (Jump)", L"W/S", L"Zoom (I/O)" };
        MainUI_Paint_DrawDropdown(hdc, pData->actionDropdownRect, pData->hFontText, L"Action", actionNames[g_selectedAction], pData->isHoveringAction);

        const wchar_t* userSafeNames[] = { L"Off", L"Legacy", L"Beta" };
        MainUI_Paint_DrawDropdown(hdc, pData->userSafeDropdownRect, pData->hFontText, L"User-Safe Mode", userSafeNames[g_userSafeMode], pData->isHoveringUserSafe);

        const wchar_t* restoreNames[] = { L"Off", L"SetForeground", L"Alt+Tab" };
        MainUI_Paint_DrawDropdown(hdc, pData->restoreMethodDropdownRect, pData->hFontText, L"Restore Window", restoreNames[g_restoreMethod], pData->isHoveringRestore);

        MainUI_Paint_DrawToggle(hdc, pData->multiInstanceToggleRect, pData->hFontText, L"Multi-Instance Support", g_multiSupport.load(), PtInRect(&pData->multiInstanceToggleRect, pData->hoverPoint), pData->multiInstanceAnim);

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }
    }
    else if (pData->currentPage == 1) { // Automation
        MainUI_Paint_DrawToggle(hdc, pData->autoStartToggleRect, pData->hFontText, L"Auto-Start AntiAFK", g_autoStartAfk.load(), PtInRect(&pData->autoStartToggleRect, pData->hoverPoint), pData->autoStartAnim);
        MainUI_Paint_DrawToggle(hdc, pData->autoReconnectToggleRect, pData->hFontText, L"Auto Reconnect", g_autoReconnect.load(), PtInRect(&pData->autoReconnectToggleRect, pData->hoverPoint), pData->autoReconnectAnim);
        MainUI_Paint_DrawToggle(hdc, pData->bloxstrapIntegrationToggleRect, pData->hFontText, L"Bloxstrap/Fishstrap Integration", g_bloxstrapIntegration.load(), PtInRect(&pData->bloxstrapIntegrationToggleRect, pData->hoverPoint), pData->bloxstrapIntegrationAnim);

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }
    }
    else if (pData->currentPage == 2) { // Statistics
        auto drawStatCard = [&](const RECT& cardRect, const wchar_t* icon, const wchar_t* label, const wchar_t* value, Color accentColor) {
            GraphicsPath path;
            REAL r = 8.0f;
            path.AddArc((REAL)cardRect.left, (REAL)cardRect.top, r * 2, r * 2, 180, 90);
            path.AddArc((REAL)cardRect.right - r * 2, (REAL)cardRect.top, r * 2, r * 2, 270, 90);
            path.AddArc((REAL)cardRect.right - r * 2, (REAL)cardRect.bottom - r * 2, r * 2, r * 2, 0, 90);
            path.AddArc((REAL)cardRect.left, (REAL)cardRect.bottom - r * 2, r * 2, r * 2, 90, 90);
            path.CloseFigure();

            Color startColor(80, accentColor.GetR(), accentColor.GetG(), accentColor.GetB());
            Color endColor(0, 45, 45, 45);
            LinearGradientBrush gradBrush(Point(cardRect.left, cardRect.top), Point(cardRect.right, cardRect.bottom), startColor, endColor);
            g.FillPath(&gradBrush, &path);

            Pen borderPen(Color(128, accentColor.GetR(), accentColor.GetG(), accentColor.GetB()), 1.5f);
            g.DrawPath(&borderPen, &path);

            HFONT iconFontH = CreateFontW(-MulDiv(28, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
            Font iconFont(hdc, iconFontH);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            RectF iconRect((REAL)cardRect.left + 15, (REAL)cardRect.top + 15, (REAL)cardRect.right - 15 - (cardRect.left + 15), (REAL)50 - 15);
            g.DrawString(icon, -1, &iconFont, iconRect, &sf, &whiteBrush);
            DeleteObject(iconFontH);

            Font valFont(hdc, pData->hFontSection);
            RectF valueRect((REAL)cardRect.left, (REAL)cardRect.top + 48, (REAL)cardRect.right - cardRect.left, (REAL)75 - 48);
            g.DrawString(value, -1, &valFont, valueRect, &sf, &whiteBrush);

            Font lblFont(hdc, pData->hFontText);
            SolidBrush grayBrush(Color(255, 200, 200, 200));
            RectF labelRect((REAL)cardRect.left, (REAL)valueRect.GetBottom(), (REAL)cardRect.right - cardRect.left, (REAL)cardRect.bottom - valueRect.GetBottom() - 5);
            g.DrawString(label, -1, &lblFont, labelRect, &sf, &grayBrush);
        };

        const int card_gap = 15;
        int card_w = (clientRect.right - content_x_start * 2 - card_gap) / 2;
        int card_h = 100;
        int y_stat = content_y_start;

        wchar_t buffer[128];

        RECT cardRect1 = { content_x_start, y_stat, content_x_start + card_w, y_stat + card_h };
        RECT cardRect2 = { cardRect1.right + card_gap, y_stat, cardRect1.right + card_gap + card_w, y_stat + card_h };
        y_stat += card_h + card_gap;
        RECT cardRect3 = { content_x_start, y_stat, content_x_start + card_w, y_stat + card_h };
        RECT cardRect4 = { cardRect3.right + card_gap, y_stat, cardRect3.right + card_gap + card_w, y_stat + card_h };

        uint64_t total_seconds = g_totalAfkTimeSeconds.load();
        if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
            total_seconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
        }
        int hours = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        swprintf_s(buffer, L"%d h %d min", hours, minutes);
        drawStatCard(cardRect1, L"\uE916", L"Total AFK Time", buffer, Color(255, 0, 122, 204));

        swprintf_s(buffer, L"%llu", g_afkActionsPerformed.load());
        drawStatCard(cardRect2, L"\uE8EE", L"Actions Performed", buffer, Color(255, 138, 43, 226));

        uint64_t lastActionTimestamp = g_lastAfkActionTimestamp.load();
        if (lastActionTimestamp > 0) {
            uint64_t elapsed = (GetTickCount64() - lastActionTimestamp) / 1000;
            if (elapsed < 60) swprintf_s(buffer, L"%llu seconds ago", elapsed);
            else if (elapsed < 3600) swprintf_s(buffer, L"%llu minutes ago", elapsed / 60);
            else swprintf_s(buffer, L"%llu hours ago", elapsed / 3600);
        } else {
            wcscpy_s(buffer, L"Never");
        } 
        drawStatCard(cardRect3, L"\uE787", L"Last Action", buffer, Color(255, 204, 51, 102));

        swprintf_s(buffer, L"%llu", g_autoReconnectsPerformed.load());
        drawStatCard(cardRect4, L"\uE7FC", L"Auto Reconnects", buffer, Color(255, 34, 177, 76)); 

        SolidBrush resetStatsBrush(pData->isHoveringResetStats ? Color(150, 90, 90, 90) : Color(100, 70, 70, 70));
        FillRoundedRectangle(&g, &resetStatsBrush, (REAL)pData->resetStatsButtonRect.left, (REAL)pData->resetStatsButtonRect.top, (REAL)(pData->resetStatsButtonRect.right - pData->resetStatsButtonRect.left), (REAL)(pData->resetStatsButtonRect.bottom - pData->resetStatsButtonRect.top), 5.0f);
        
        Font sectionFont(hdc, pData->hFontSection);
        SolidBrush textBrush(Color(255, 220, 220, 220));
        RectF resetStatsRectF((REAL)pData->resetStatsButtonRect.left, (REAL)pData->resetStatsButtonRect.top, (REAL)(pData->resetStatsButtonRect.right - pData->resetStatsButtonRect.left), (REAL)(pData->resetStatsButtonRect.bottom - pData->resetStatsButtonRect.top));
        g.DrawString(L"Reset Statistics", -1, &sectionFont, resetStatsRectF, &sf, &textBrush);

    }
    else if (pData->currentPage == 3) { // Advanced
        MainUI_Paint_DrawToggle(hdc, pData->fishstrapToggleRect, pData->hFontText, L"FishStrap/Shaders Support", g_fishstrapSupport.load(), PtInRect(&pData->fishstrapToggleRect, pData->hoverPoint), pData->fishstrapAnim);
        MainUI_Paint_DrawToggle(hdc, pData->autoUpdateToggleRect, pData->hFontText, L"Update Checker", g_autoUpdate.load(), PtInRect(&pData->autoUpdateToggleRect, pData->hoverPoint), pData->autoUpdateAnim);
        MainUI_Paint_DrawToggle(hdc, pData->legacyUiToggleRect, pData->hFontText, L"Use Legacy UI (Tray)", g_useLegacyUi.load(), PtInRect(&pData->legacyUiToggleRect, pData->hoverPoint), pData->legacyUiAnim);

            wchar_t fpsLabel[48];
            if (g_fpsLimit == 0) swprintf_s(fpsLabel, L"Off");
            else swprintf_s(fpsLabel, L"%d FPS", g_fpsLimit);
            MainUI_Paint_DrawDropdown(hdc, pData->fpsCapperDropdownRect, pData->hFontText, L"FPS Capper*", fpsLabel, pData->isHoveringFpsCapper);

        MainUI_Paint_DrawToggle(hdc, pData->unlockFpsOnFocusToggleRect, pData->hFontText, L"Unlock FPS when focus", g_unlockFpsOnFocus.load(), PtInRect(&pData->unlockFpsOnFocusToggleRect, pData->hoverPoint), pData->unlockFpsOnFocusAnim);

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }

        SolidBrush resetBrush(pData->isHoveringReset ? Color(150, 90, 90, 90) : Color(100, 70, 70, 70));
        FillRoundedRectangle(&g, &resetBrush, (REAL)pData->resetSettingsButtonRect.left, (REAL)pData->resetSettingsButtonRect.top, (REAL)(pData->resetSettingsButtonRect.right - pData->resetSettingsButtonRect.left), (REAL)(pData->resetSettingsButtonRect.bottom - pData->resetSettingsButtonRect.top), 5.0f);
        
        Font sectionFont(hdc, pData->hFontSection);
        SolidBrush textBrush(Color(255, 220, 220, 220));
        RectF resetRectF((REAL)pData->resetSettingsButtonRect.left, (REAL)pData->resetSettingsButtonRect.top, (REAL)(pData->resetSettingsButtonRect.right - pData->resetSettingsButtonRect.left), (REAL)(pData->resetSettingsButtonRect.bottom - pData->resetSettingsButtonRect.top));
        g.DrawString(L"Reset All Settings", -1, &sectionFont, resetRectF, &sf, &textBrush);
    }
    
    SelectObject(hdc, oldFont);

    SelectObject(hdc, pData->hFontSmall);
    SetTextColor(hdc, RGB(128, 128, 128));

    RECT disclaimerRect = { 0, clientRect.bottom - 25, clientRect.right, clientRect.bottom - 5 };
    const wchar_t* disclaimerText;
    if (pData->currentPage == 2) {
        disclaimerText = L"Statistics are saved only when the application is closed correctly.";
    } else {
        disclaimerText = L"Closing this window minimizes it to the system tray.";
    }
    Font smallFont(hdc, pData->hFontSmall);
    SolidBrush disclaimerBrush(Color(255, 128, 128, 128));
    RectF disclaimerRectF((REAL)0, (REAL)clientRect.bottom - 25, (REAL)clientRect.right, (REAL)20);
    g.DrawString(disclaimerText, -1, &smallFont, disclaimerRectF, &sf, &disclaimerBrush);
}
LRESULT CALLBACK MainUIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MainUIData* pData = (MainUIData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new MainUIData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        pData->hFontTitle = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontSection = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontSmall = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontNav = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontNavSelected = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int margin = 20, rowH = 32, vGap = 15, toggleW = 50;
        const int nav_bar_y = 30, nav_bar_h = 30;
        const int content_y_start = nav_bar_y + nav_bar_h + 20;
        int y = content_y_start + 60;
        
        pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, 30 };
        pData->iconButtonRect = { 0, 0, 46, 30 };
        pData->startButtonRect = { margin, content_y_start, clientRect.right - margin, content_y_start + 40 };
        
        PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(clientRect.right, clientRect.bottom));

        pData->navItems.push_back({ L"General", {0, 0, 0, 0} }); 
        pData->navItems.push_back({ L"Automation", {0, 0, 0, 0} });
        pData->navItems.push_back({ L"Statistics", {0, 0, 0, 0} });
        pData->navItems.push_back({ L"Advanced", {0, 0, 0, 0} });


        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);

        pData->hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_OFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

        pData->multiInstanceAnim = g_multiSupport.load() ? 1.0f : 0.0f;
        pData->fishstrapAnim = g_fishstrapSupport.load() ? 1.0f : 0.0f;
        pData->autoUpdateAnim = g_autoUpdate.load() ? 1.0f : 0.0f;
        pData->autoStartAnim = g_autoStartAfk.load() ? 1.0f : 0.0f;
        pData->autoReconnectAnim = g_autoReconnect.load() ? 1.0f : 0.0f;
        pData->bloxstrapIntegrationAnim = g_bloxstrapIntegration.load() ? 1.0f : 0.0f;
        pData->unlockFpsOnFocusAnim = g_unlockFpsOnFocus.load() ? 1.0f : 0.0f;

        pData->previousAfkStartedState = g_isAfkStarted.load();
        pData->stateChangeProgress = 1.0f;

        pData->legacyUiAnim = g_useLegacyUi.load() ? 1.0f : 0.0f;

        return 0;

    }
    case WM_TIMER: {
        if (wParam == 1001) {
            if (pData && g_hMainUiWnd && IsWindow(g_hMainUiWnd) && pData->currentPage == 2) {
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }

    case WM_APP + 11:
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        return 0;

    case WM_SIZE:
    {
        if (!pData) break;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int margin = 20, rowH = 32, vGap = 15, toggleW = 50;
        const int nav_bar_y = 30, nav_bar_h = 30;
        const int content_y_start = nav_bar_y + nav_bar_h + 20;
        int y = content_y_start + 40 + 20; 

        const int nav_item_w = (clientRect.right / 4);
        pData->navItems[0].second = { 0 * nav_item_w, nav_bar_y, 1 * nav_item_w, nav_bar_y + nav_bar_h };
        pData->navItems[1].second = { 1 * nav_item_w, nav_bar_y, 2 * nav_item_w, nav_bar_y + nav_bar_h };
        pData->navItems[2].second = { 2 * nav_item_w, nav_bar_y, 3 * nav_item_w, nav_bar_y + nav_bar_h };
        pData->navItems[3].second = { 3 * nav_item_w, nav_bar_y, clientRect.right, nav_bar_y + nav_bar_h };

        int help_btn_size = 24;
        int help_btn_margin = 8;
        int helpX = clientRect.right - margin - help_btn_size;

        int control_label_width = 160;
        int control_area_start_x = margin + control_label_width;
        int control_area_width = clientRect.right - margin - control_area_start_x - help_btn_size - help_btn_margin;

        int dropW = control_area_width;

        pData->helpButtonRects.clear();
        if (pData->currentPage == 0) { // General
            pData->intervalDropdownRect = { control_area_start_x, y, control_area_start_x + dropW, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->actionDropdownRect = { control_area_start_x, y, control_area_start_x + dropW, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->multiInstanceToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->userSafeDropdownRect = { control_area_start_x, y, control_area_start_x + dropW, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->restoreMethodDropdownRect = { control_area_start_x, y, control_area_start_x + dropW, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
        } else if (pData->currentPage == 1) { // Automation
            pData->autoStartToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->autoReconnectToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->bloxstrapIntegrationToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
        } else if (pData->currentPage == 2) { // Statistics
            pData->resetStatsButtonRect = { margin, clientRect.bottom - 40 - 30, clientRect.right - margin, clientRect.bottom - 30 };
        } else if (pData->currentPage == 3) { // Advanced
            pData->fishstrapToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->autoUpdateToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->legacyUiToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->fpsCapperDropdownRect = { control_area_start_x, y, control_area_start_x + dropW, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->unlockFpsOnFocusToggleRect = { helpX - help_btn_margin - toggleW, y, helpX - help_btn_margin, y + rowH }; pData->helpButtonRects.push_back({ helpX, y + (rowH - help_btn_size) / 2, helpX + help_btn_size, y + (rowH - help_btn_size) / 2 + help_btn_size }); y += rowH + vGap;
            pData->resetSettingsButtonRect = { margin, clientRect.bottom - 40 - 30, clientRect.right - margin, clientRect.bottom - 30 };
        }
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }

    case WM_MOUSEMOVE:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        pData->hoverPoint = pt;
        auto checkHover = [&](bool& flag, const RECT& rect) {
            bool now = PtInRect(&rect, pt);
            if (now != flag) { flag = now; InvalidateRect(hwnd, &rect, FALSE); }
            return now;
        };

        bool anyHover = false;
        anyHover |= checkHover(pData->isHoveringIcon, pData->iconButtonRect);
        anyHover |= checkHover(pData->isHoveringClose, pData->closeButtonRect);
        anyHover |= checkHover(pData->isHoveringStart, pData->startButtonRect);
        if (pData->currentPage == 2) {
            anyHover |= checkHover(pData->isHoveringResetStats, pData->resetStatsButtonRect);
        }
        anyHover |= checkHover(pData->isHoveringReset, pData->resetSettingsButtonRect);
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringInterval, pData->intervalDropdownRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringAction, pData->actionDropdownRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringUserSafe, pData->userSafeDropdownRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringRestore, pData->restoreMethodDropdownRect));
        anyHover |= (pData->currentPage == 3 && checkHover(pData->isHoveringFpsCapper, pData->fpsCapperDropdownRect));
        anyHover |= PtInRect(&pData->multiInstanceToggleRect, pt) && pData->currentPage == 0;
        anyHover |= PtInRect(&pData->fishstrapToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->autoUpdateToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->autoStartToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoReconnectToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->bloxstrapIntegrationToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->unlockFpsOnFocusToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->legacyUiToggleRect, pt) && pData->currentPage == 3;

        int newHoveringNavItem = -1;
        for (size_t i = 0; i < pData->navItems.size(); ++i) {
            if (PtInRect(&pData->navItems[i].second, pt)) {
                newHoveringNavItem = i;
                break;
            }
        } 
        if (newHoveringNavItem != pData->hoveringNavItem) { pData->hoveringNavItem = newHoveringNavItem; InvalidateRect(hwnd, NULL, FALSE); }
        anyHover |= (newHoveringNavItem != -1);

        int newHoveringHelpButton = -1;
        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            if (PtInRect(&pData->helpButtonRects[i], pt)) {
                newHoveringHelpButton = i;
                break;
            }
        }
        if (newHoveringHelpButton != pData->hoveringHelpButton) { pData->hoveringHelpButton = newHoveringHelpButton; InvalidateRect(hwnd, NULL, FALSE); anyHover = true; }
        anyHover |= (newHoveringHelpButton != -1);

        SetCursor(anyHover ? pData->hCursorHand : pData->hCursorArrow);

        if (anyHover && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        if (anyHover) {
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringIcon = pData->isHoveringClose = pData->isHoveringStart = pData->isHoveringReset = pData->isHoveringInterval = pData->isHoveringAction = pData->isHoveringUserSafe = pData->isHoveringRestore = false;
        pData->hoveringNavItem = -1; pData->isHoveringResetStats = false;
        pData->isHoveringFpsCapper = false; 
        pData->hoveringHelpButton = -1;
        pData->isTrackingMouse = false;
        if (pData->isPressingStart) {
            pData->isPressingStart = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        InvalidateRect(hwnd, NULL, FALSE);
        SetCursor(pData->hCursorArrow);
        return 0;
    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt)) { PostMessage(hwnd, WM_CLOSE, 0, 0); return 0; }
        MainUI_HandleClick(hwnd, pt, pData);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt))
        {
            ShowWindow(hwnd, SW_MINIMIZE);
            return 0;
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        if (pData && pData->isPressingStart) {
            pData->isPressingStart = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_NCLBUTTONUP:
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            RECT dragRect = { pData->iconButtonRect.right, 0, pData->closeButtonRect.left, 30 };
            if (PtInRect(&dragRect, pt)) return HTCAPTION;
        }
        return hit;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        if (!pData) break;

        bool needsRedraw = false;

        if (pData->errorAnimationDirection == 1) { 
            pData->errorAnimationProgress += 0.04f; 
            if (pData->errorAnimationProgress >= 1.0f) {
                pData->errorAnimationProgress = 1.0f;
                pData->errorAnimationDirection = 0;
            }
        } else if (pData->errorAnimationDirection == -1) { 
            pData->errorAnimationProgress -= 0.04f; 
            if (pData->errorAnimationProgress <= 0.0f) {
                pData->errorAnimationProgress = 0.0f;
                pData->errorAnimationDirection = 0;
                pData->startButtonErrorText.clear();
            }
        } else if (pData->errorAnimationProgress == 1.0f) { 
            if (GetTickCount64() - pData->startButtonErrorTime >= 3000) {
                pData->errorAnimationDirection = -1; 
            }
        }

        if (pData->stateChangeDirection == 1) {
            pData->stateChangeProgress += 0.05f;
            if (pData->stateChangeProgress >= 1.0f) {
                pData->stateChangeProgress = 1.0f;
                pData->stateChangeDirection = 0;
                pData->previousAfkStartedState = g_isAfkStarted.load();
            } else {
                needsRedraw = true;
            }
        }
       
        needsRedraw = needsRedraw || (pData->errorAnimationDirection != 0) || (pData->stateChangeDirection != 0);
        
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        const float spring = 0.1f;
        const float friction = 0.75f;
        float targetScale = pData->isPressingStart ? 0.92f : 1.0f;
        float force = (targetScale - pData->startButtonScale) * spring;
        pData->startButtonVelocity += force;
        pData->startButtonVelocity *= friction;
        pData->startButtonScale += pData->startButtonVelocity;
        if (abs(pData->startButtonScale - targetScale) > 0.001f || abs(pData->startButtonVelocity) > 0.001f) {
            if (!needsRedraw) needsRedraw = true;
        }

        const float animSpeed = 0.2f;
        auto animateToggle = [&](float& val, bool target) {
            float targetVal = target ? 1.0f : 0.0f;
            if (abs(val - targetVal) > 0.001f) {
                val += (targetVal - val) * animSpeed;
                needsRedraw = true;
                if (abs(val - targetVal) < 0.01f) val = targetVal;
            }
        };
        auto animateFloat = [&](float& val, float targetVal) {
            if (abs(val - targetVal) > 0.5f) {
                val += (targetVal - val) * animSpeed;
            } else {
                val = targetVal;
            } 
        };

        if (pData->currentPage >= 0 && pData->currentPage < pData->navItems.size()) {
            const RECT& targetRect = pData->navItems[pData->currentPage].second;
            float targetX = (float)targetRect.left;
            float targetW = (float)(targetRect.right - targetRect.left);
            animateFloat(pData->navIndicatorX, targetX);
            animateFloat(pData->navIndicatorWidth, targetW);
        }
        if (abs(pData->navIndicatorX - (float)pData->navItems[pData->currentPage].second.left) > 0.5f) needsRedraw = true;


        animateToggle(pData->multiInstanceAnim, g_multiSupport.load());
        animateToggle(pData->fishstrapAnim, g_fishstrapSupport.load());
        animateToggle(pData->autoUpdateAnim, g_autoUpdate.load());
        animateToggle(pData->autoStartAnim, g_autoStartAfk.load());
        animateToggle(pData->autoReconnectAnim, g_autoReconnect.load());
        animateToggle(pData->unlockFpsOnFocusAnim, g_unlockFpsOnFocus.load());
        animateToggle(pData->bloxstrapIntegrationAnim, g_bloxstrapIntegration.load());
        animateToggle(pData->legacyUiAnim, g_useLegacyUi.load());
        FillRect(memDC, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        SetBkMode(memDC, TRANSPARENT);





        MainUI_Paint_DrawContent(memDC, clientRect, pData);
        MainUI_Paint_DrawCloseButton(memDC, pData->closeButtonRect, pData->isHoveringClose);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);
        if (needsRedraw && pData->uTimerId == 0) {
            pData->uTimerId = timeSetEvent(16, 1, [](UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {

                if (IsWindow((HWND)dwUser)) {
                    InvalidateRect((HWND)dwUser, NULL, FALSE);
                }
                }, (DWORD_PTR)hwnd, TIME_PERIODIC);

        } else if (!needsRedraw && pData->uTimerId != 0) {
            timeKillEvent(pData->uTimerId);
            pData->uTimerId = 0;
        }


        if (g_hMainUiWnd && IsWindow(g_hMainUiWnd) && pData->currentPage == 2) {
            SetTimer(hwnd, 1001, 1000, NULL);
        } 
        else {
            KillTimer(hwnd, 1001);
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        if (pData) {
            if (pData->uTimerId != 0) {
                timeKillEvent(pData->uTimerId);
            }
            DeleteObject(pData->hFontTitle); DeleteObject(pData->hFontSection);
            DeleteObject(pData->hFontText); DeleteObject(pData->hFontSmall);
            DeleteObject(pData->hFontNav); DeleteObject(pData->hFontNavSelected);
            DestroyCursor(pData->hCursorHand); DestroyCursor(pData->hCursorArrow);
            if (pData->hIcon) DestroyIcon(pData->hIcon);
            delete pData;

            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        g_hMainUiWnd = NULL;
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
// ==========

// Dark MessageBox Window
struct DarkMessageBoxParams
{
    const wchar_t* text;
    UINT type;
    int* result;
};
LRESULT CALLBACK DarkMessageBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    struct DarkMessageBoxData {
        DarkMessageBoxParams* params = nullptr;
        HFONT hFontTitle = NULL, hFontText = NULL;
        RECT closeButtonRect = { 0 };
        std::vector<std::pair<int, RECT>> buttonRects;
        bool isHoveringClose = false;
        int hoveringButton = -1;
        bool isTrackingMouse = false;
        HCURSOR hCursorHand = NULL, hCursorArrow = NULL;
        HICON hIcon = NULL;
    };

    DarkMessageBoxData* pData = (DarkMessageBoxData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new DarkMessageBoxData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        EnableAcrylic(hwnd);
        enum DWM_WINDOW_CORNER_PREFERENCE { DWMWCP_ROUND = 2 };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        pData->params = (DarkMessageBoxParams*)lpcs->lpCreateParams;

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        pData->hFontTitle = CreateFontW(-MulDiv(11, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        HDC hdc = CreateCompatibleDC(screen);
        ReleaseDC(NULL, screen);
        HFONT oldFont = (HFONT)SelectObject(hdc, pData->hFontText);

        const int main_margin = 20;
        const int button_margin = 10;
        const int icon_size = 32;
        const int icon_text_gap = 15;
        int dlgW = 380;

        RECT textRect = { 0, 0, dlgW - main_margin * 2 - icon_size - icon_text_gap, 0 };
        DrawTextW(hdc, pData->params->text, -1, &textRect, DT_CALCRECT | DT_WORDBREAK);
        SelectObject(hdc, oldFont);
        DeleteDC(hdc);

        int textH = textRect.bottom;
        int contentH = max(textH, icon_size);

        const int btnH = 30;
        const int button_pane_height = btnH + 2 * button_margin;
        int dlgH = 30 + main_margin + contentH + button_margin + button_pane_height;

        UINT iconType = pData->params->type & 0xF0;
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
            pData->hIcon = LoadIcon(NULL, iconId);
        }

        UINT btnType = pData->params->type & 0x0F;
        std::vector<std::pair<int, const wchar_t*>> buttons;
        if (btnType == MB_OK) {
            buttons.push_back({ IDOK, L"OK" });
        }
        else if (btnType == MB_OKCANCEL)
        {
            buttons.push_back({ IDOK, L"OK" });
            buttons.push_back({ IDCANCEL, L"Cancel" });
        }
        else if (btnType == MB_YESNO)
        {
            buttons.push_back({ IDYES, L"Yes" });
            buttons.push_back({ IDNO, L"No" });
        }
        else if (btnType == MB_YESNOCANCEL)
        {
            buttons.push_back({ IDYES, L"Yes" });
            buttons.push_back({ IDNO, L"No" });
            buttons.push_back({ IDCANCEL, L"Cancel" });
        }
        else
        {
            buttons.push_back({ IDOK, L"OK" });
        }

        const int btnW = 90;
        const int btnGap = 10;
        int totalBtnW = buttons.size() * btnW + (buttons.size() - 1) * btnGap;
        int btnY = dlgH - button_pane_height + button_margin;
        int currentX = dlgW - main_margin - totalBtnW;

        for (size_t i = 0; i < buttons.size(); ++i)
        {
            const auto& btn = buttons[i];
            bool isDefault = false;
            UINT defBtn = (pData->params->type & MB_DEFMASK);
            if ((defBtn == MB_DEFBUTTON1 && i == 0) || (defBtn == MB_DEFBUTTON2 && i == 1) || (defBtn == MB_DEFBUTTON3 && i == 2))
            {
                isDefault = true;
            }
            RECT btnRect = { currentX, btnY, currentX + btnW, btnY + btnH };
            pData->buttonRects.push_back({ btn.first, btnRect });
            if (isDefault)
                SetFocus(hwnd); 
            currentX += btnW + btnGap;
        }

        pData->closeButtonRect = { dlgW - 46, 0, dlgW, 30 };

        RECT wndRect = { 0, 0, dlgW, dlgH };
        AdjustWindowRect(&wndRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);
        int finalW = wndRect.right - wndRect.left;
        int finalH = wndRect.bottom - wndRect.top;

        int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - finalW) / 2, y = (screenH - finalH) / 2;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, finalW, finalH, 0);

        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);

        return 0;
    }
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            RECT dragRect = { 0, 0, pData->closeButtonRect.left, 30 };
            if (PtInRect(&dragRect, pt)) return HTCAPTION;
        }
        return hit;
    }
    case WM_MOUSEMOVE:
    {
        if (!pData) break;
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        bool nowHoveringClose = PtInRect(&pData->closeButtonRect, pt);
        if (nowHoveringClose != pData->isHoveringClose) {
            pData->isHoveringClose = nowHoveringClose;
            InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        }

        int nowHoveringButton = -1;
        for (size_t i = 0; i < pData->buttonRects.size(); ++i) {
            if (PtInRect(&pData->buttonRects[i].second, pt)) {
                nowHoveringButton = pData->buttonRects[i].first;
                break;
            }
        }

        if (nowHoveringButton != pData->hoveringButton) {
            if (pData->hoveringButton != -1) {
                for (const auto& btn : pData->buttonRects) {
                    if (btn.first == pData->hoveringButton) {
                        InvalidateRect(hwnd, &btn.second, FALSE);
                        break;
                    }
                }
            }
            pData->hoveringButton = nowHoveringButton;
            if (pData->hoveringButton != -1) {
                for (const auto& btn : pData->buttonRects) {
                    if (btn.first == pData->hoveringButton) {
                        InvalidateRect(hwnd, &btn.second, FALSE);
                        break;
                    }
                }
            }
        }

        bool anyHover = nowHoveringClose || (nowHoveringButton != -1);
        SetCursor(anyHover ? pData->hCursorHand : pData->hCursorArrow);

        if (anyHover && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        if (!pData) break;
        pData->isHoveringClose = false;
        pData->hoveringButton = -1;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, NULL, FALSE);
        SetCursor(pData->hCursorArrow);
        return 0;
    case WM_LBUTTONDOWN:
    {
        if (!pData) break;
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&pData->closeButtonRect, pt)) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        for (const auto& btn : pData->buttonRects) {
            if (PtInRect(&btn.second, pt)) {
                PostMessage(hwnd, WM_COMMAND, btn.first, 0);
                return 0;
            }
        }
        break;
    }
    case WM_PAINT:
    {
        if (!pData) break;
        PAINTSTRUCT ps;
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        EnableAcrylic(hwnd); 
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HGDIOBJ oldBMP = SelectObject(memDC, memBMP);

        Graphics g(memDC);
        g.SetSmoothingMode(SmoothingModeAntiAlias);

        SolidBrush bgBrush(Color(100, 10, 10, 10));
        g.FillRectangle(&bgBrush, (REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)clientRect.bottom);

        SolidBrush headerPaneBrush(Color(120, 20, 20, 20));
        g.FillRectangle(&headerPaneBrush, (REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)30);
        Pen headerPen(Color(15, 255, 255, 255), 1);
        g.DrawLine(&headerPen, (REAL)0, (REAL)30, (REAL)clientRect.right, (REAL)30);
        
        HICON hAppIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_OFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
        if (hAppIcon) {
             DrawIconEx(memDC, 15, 7, hAppIcon, 16, 16, 0, NULL, DI_NORMAL);
             DestroyIcon(hAppIcon);
        }

        wchar_t caption[256];
        GetWindowTextW(hwnd, caption, 128);
        Font titleFont(memDC, pData->hFontTitle);
        SolidBrush titleBrush(Color(255, 255, 255, 255));
        
        StringFormat sfCenter;
        sfCenter.SetAlignment(StringAlignmentCenter);
        sfCenter.SetLineAlignment(StringAlignmentCenter);
        RectF titleRectF((REAL)0, (REAL)0, (REAL)clientRect.right, (REAL)30); 
        g.DrawString(caption, -1, &titleFont, titleRectF, &sfCenter, &titleBrush);

        Font textFont(memDC, pData->hFontText);
        SolidBrush textBrush(Color(255, 220, 220, 220));
        RectF textRectF((REAL)20, (REAL)40, (REAL)(clientRect.right - 40), (REAL)(clientRect.bottom - 50));
        
        StringFormat sf;
        sf.SetAlignment(StringAlignmentNear);
        sf.SetLineAlignment(StringAlignmentNear);
        g.DrawString(pData->params->text, -1, &textFont, textRectF, &sf, &textBrush);

        if (!pData->buttonRects.empty()) {
             SolidBrush paneBrush(Color(120, 20, 20, 20));
             g.FillRectangle(&paneBrush, (REAL)0, (REAL)(pData->buttonRects[0].second.top - 10), (REAL)clientRect.right, (REAL)(clientRect.bottom - (pData->buttonRects[0].second.top - 10)));
        }

        auto drawBtn = [&](const RECT& r, const wchar_t* txt, bool hover, bool primary) {
             Color fill = primary ? (hover ? Color(255, 20, 142, 224) : Color(255, 0, 122, 204)) : (hover ? Color(255, 80, 80, 80) : Color(255, 60, 60, 60));
             SolidBrush br(fill);
             GraphicsPath path;
             path.AddArc((REAL)r.left, (REAL)r.top, 5.0f * 2, 5.0f * 2, 180, 90);
             path.AddArc((REAL)r.right - 5.0f * 2, (REAL)r.top, 5.0f * 2, 5.0f * 2, 270, 90);
             path.AddArc((REAL)r.right - 5.0f * 2, (REAL)r.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 0, 90);
             path.AddArc((REAL)r.left, (REAL)r.bottom - 5.0f * 2, 5.0f * 2, 5.0f * 2, 90, 90);
             path.CloseFigure();
             g.FillPath(&br, &path);

             Font font(memDC, pData->hFontText);
             SolidBrush btnTextBrush(Color(255, 255, 255, 255));
             StringFormat sf;
             sf.SetAlignment(StringAlignmentCenter);
             sf.SetLineAlignment(StringAlignmentCenter);
             RectF btnRectF((REAL)r.left, (REAL)r.top, (REAL)(r.right - r.left), (REAL)(r.bottom - r.top));
             g.DrawString(txt, -1, &font, btnRectF, &sf, &btnTextBrush);
        };

        for (const auto& btn : pData->buttonRects) {
            const wchar_t* text = L"";
            if (btn.first == IDOK) text = L"OK";
            else if (btn.first == IDCANCEL) text = L"Cancel";
            else if (btn.first == IDYES) text = L"Yes";
            else if (btn.first == IDNO) text = L"No";

            bool isPrimary = (btn.first == IDOK || btn.first == IDYES);
            drawBtn(btn.second, text, pData->hoveringButton == btn.first, isPrimary);
        }

        DrawSharedCloseButton(&g, pData->closeButtonRect, pData->isHoveringClose);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_COMMAND:
        if (pData && pData->params && pData->params->result)
        {
            *(pData->params->result) = LOWORD(wParam);
        }
        DestroyWindow(hwnd);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            if (pData && pData->params && pData->params->result)
            {
                UINT btnType = pData->params->type & 0x0F;
                if (btnType == MB_OKCANCEL || btnType == MB_YESNOCANCEL)
                    *(pData->params->result) = IDCANCEL;
            }
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        if (pData && pData->params && pData->params->result)
        {
            UINT btnType = pData->params->type & 0x0F;
            if (btnType == MB_OKCANCEL || btnType == MB_YESNOCANCEL)
                *(pData->params->result) = IDCANCEL;
        }
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        if (pData) {
            if (pData->hFontTitle) DeleteObject(pData->hFontTitle);
            if (pData->hFontText) DeleteObject(pData->hFontText);
            if (pData->hCursorHand) DestroyCursor(pData->hCursorHand);
            if (pData->hCursorArrow) DestroyCursor(pData->hCursorArrow);
            if (pData->hIcon) DestroyIcon(pData->hIcon);
            delete pData;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
int ShowDarkMessageBox(HWND owner, const wchar_t* text, const wchar_t* caption, UINT type)
{
    const wchar_t DLG_CLASS_NAME[] = L"AntiAFK-RBX-DarkMessageBox";
    WNDCLASS wc = { 0 };
    if (!GetClassInfo(g_hInst, DLG_CLASS_NAME, &wc))
    {
        wc.lpfnWndProc = DarkMessageBoxProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = DLG_CLASS_NAME;
        wc.hbrBackground = CreateSolidBrush(DARK_BG);
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
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

    DarkMessageBoxParams params = { text, type, &result };

    HWND hDialog = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, DLG_CLASS_NAME, caption, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, owner, NULL, g_hInst, &params);
    if (hDialog)
    {   
        ShowWindow(hDialog, SW_SHOW);
        UpdateWindow(hDialog);
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
// ==========

// Main thread
void main_thread(bool arg_tray)
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
    if (g_userSafeMode > 0)
    {
        StartActivityMonitor();
    }

    UpdateSplashStatus(L"Finalizing setup...");
    if (g_autoStartAfk.load())
    {
        auto wins = FindAllRobloxWindows(true);
        if (!wins.empty())
        {
            UpdateSplashStatus(L"Auto-starting Anti-AFK...");
            g_isAfkStarted = true;
            CreateTrayMenu(true);
            UpdateTrayIcon();
        }
        else
        {
            UpdateSplashStatus(L"Roblox not found for auto-start.");
        }
    }

    if (g_fpsLimit > 0) {
        UpdateSplashStatus(L"Starting FPS Capper...");
        if (g_fpsCapperThread.joinable()) g_fpsCapperThread.join();
        g_fpsCapperThread = std::thread(FpsCapperThread);
    }

    Sleep(500);
    UpdateSplashStatus(L"Loaded!");
    Sleep(1200);
    if (g_hSplashWnd)
    {
        AnimateWindow(g_hSplashWnd, 200, AW_BLEND | AW_HIDE);
        DestroyWindow(g_hSplashWnd);
    }

    if (!arg_tray) { 
        if (!g_tutorialShown.load()) {
            PostMessage(g_hwnd, WM_APP + 2, 0, 0); 
        } else if (!g_useLegacyUi.load()) {
            PostMessage(g_hwnd, WM_COMMAND, ID_OPEN_UI, 0);
        }
    }

    while (!g_stopThread.load())
    {
        std::unique_lock<std::mutex> lock(g_cv_m);
        if (g_isAfkStarted.load())
        {
            HWND user = GetForegroundWindow();
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                if (g_autoStartAfk.load() && g_isAfkStarted.load())
                { 
                    if (g_afkStartTime.load() > 0) {
                        g_totalAfkTimeSeconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
                        g_afkStartTime = 0;
                    }
                    g_isAfkStarted = false; CreateTrayMenu(false); UpdateTrayIcon(); ShowTrayNotification(L"AntiAFK-RBX | Auto-Stopped", L"Roblox window not found. AntiAFK is now off."); continue; 
                }
                else
                {
                    ShowTrayNotification(L"AntiAFK Error", L"Roblox window not found!");
                }
            }
            else
            {
                if (g_userSafeMode > 0)
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

                        g_isFpsCapperPaused = true;
                        Sleep(50);
                        SetForegroundWindow(w);
                        if (g_autoReconnect.load()) {
                            if (CheckForAutoReconnect(w))
                                g_autoReconnectsPerformed++;
                        }
                        for (int j = 0; j < 3; j++)
                        {
                            AntiAFK_Action(w);
                        }

                        if (wasMinimized)
                            ShowWindow(w, SW_MINIMIZE);
                        g_isFpsCapperPaused = false;
                    }
                }
                else
                {
                    HWND w = wins.front();
                    bool wasMinimized = IsIconic(w);
                    if (wasMinimized)
                        ShowWindow(w, SW_RESTORE);

                    g_isFpsCapperPaused = true;
                    Sleep(50);
                    SetForegroundWindow(w);
                    if (g_autoReconnect.load()) {
                        if (CheckForAutoReconnect(w))
                            g_autoReconnectsPerformed++;
                    }
                    for (int i = 0; i < 3; i++)
                    {
                        AntiAFK_Action(w);
                    }

                    if (wasMinimized)
                    {
                        ShowWindow(w, SW_MINIMIZE);
                    }
                    g_isFpsCapperPaused = false;
                }

                if (g_restoreMethod > 0)
                {
                    if (g_restoreMethod == 1)
                    {
                        RestoreForegroundWindow(user);
                    }
                    else if (g_restoreMethod == 2)
                    {
                        Sleep(50);
                        RestorePreviousWindowWithAltTab();
                    }
                }
                g_afkActionsPerformed++;
                g_lastAfkActionTimestamp = GetTickCount64();
            }
            g_updateInterval = false;
            g_cv.wait_for(lock, std::chrono::seconds(g_selectedTime), [] { return g_stopThread.load() || g_updateInterval.load() || !g_isAfkStarted.load(); });
        }
        else
        {
            if (g_autoStartAfk.load())
            {
                auto wins = FindAllRobloxWindows(true);
                if (!wins.empty()) {
                    bool shouldStart = false;
                    if (g_manuallyStoppedPids.empty()) {
                        shouldStart = true;
                    } else {
                        bool anyOldProcessExists = false;
                        std::vector<DWORD> stillRunningPids;
                        for (DWORD pid : g_manuallyStoppedPids) {
                            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                            if (hProcess) {
                                DWORD exitCode;
                                if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                                    anyOldProcessExists = true;
                                    stillRunningPids.push_back(pid);
                                }
                                CloseHandle(hProcess);
                            }
                        }
                        g_manuallyStoppedPids = stillRunningPids;

                        if (!anyOldProcessExists) {
                            shouldStart = true;
                        } else {
                            std::vector<DWORD> currentPids;
                            for (HWND w : wins) {
                                DWORD pid; GetWindowThreadProcessId(w, &pid);
                                if (std::find(currentPids.begin(), currentPids.end(), pid) == currentPids.end()) {
                                    currentPids.push_back(pid);
                                }
                            }
                            std::sort(currentPids.begin(), currentPids.end());
                            std::sort(g_manuallyStoppedPids.begin(), g_manuallyStoppedPids.end());
                            if (currentPids != g_manuallyStoppedPids) {
                                shouldStart = true;
                                g_manuallyStoppedPids.clear(); 
                            }
                        }
                    }

                    if (shouldStart) {
                        if (!g_isAfkStarted.load()) {
                            g_isAfkStarted = true;
                            g_afkStartTime = GetTickCount64();
                            CreateTrayMenu(true);
                            UpdateTrayIcon();
                            ShowTrayNotification(L"AntiAFK-RBX | Auto-Started", L"AntiAFK has started automatically.");
                        }
                        if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                            MainUIData* pData = (MainUIData*)GetWindowLongPtr(g_hMainUiWnd, GWLP_USERDATA);
                            if (pData) {
                                pData->previousAfkStartedState = false;
                                pData->stateChangeProgress = 0.0f;
                                pData->stateChangeDirection = 1;
                            }
                            InvalidateRect(g_hMainUiWnd, &pData->startButtonRect, FALSE);

                         }

                        g_cv.notify_one(); 
                        continue; 
                    }
                } else {
                    if (!g_manuallyStoppedPids.empty()) {
                        g_manuallyStoppedPids.clear();
                    }
                }
            }
            g_cv.wait_for(lock, 5000ms, [] { return g_stopThread.load() || g_isAfkStarted.load(); });
        }
    }
    StopActivityMonitor();
}
// ==========

// Main tray worker & winapi worker
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
        lstrcpy(g_nid.szTip, L"AntiAFK-RBX • Click right mouse button for menu");
        Shell_NotifyIcon(NIM_ADD, &g_nid);
        CreateTrayMenu(g_isAfkStarted.load());
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        break;
    case WM_SHOWWINDOW:
        if (wParam == TRUE)
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        break;
    case WM_APP + 12: 
        {
            int result = ShowDarkMessageBox(hwnd, L"Are you sure you want to reset all statistics?\nThis action cannot be undone.", L"Reset Statistics", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            if (result == IDYES) {
                g_totalAfkTimeSeconds = 0;
                g_afkActionsPerformed = 0;
                g_autoReconnectsPerformed = 0;
                SaveSettings(); 
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
            }
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
        else if (lParam == WM_LBUTTONDOWN)
        {
            if (!g_useLegacyUi.load())
                ShowMainUIDialog(hwnd);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SHOW_TUTORIAL:
        {
            ShowTutorialDialog(hwnd);
            break;
        }
        case ID_OPEN_UI:
        {
            ShowMainUIDialog(hwnd);
            break;
        }
        case ID_ABOUT_MENU:
        {
            ShowAboutDialog(hwnd);
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
                if (!g_isAfkStarted.load())
                {
                    g_afkStartTime = GetTickCount64();
                }
                ShowTrayNotification(L"AntiAFK Started", L"AntiAFK has started.");
                g_isAfkStarted = true;
                g_manuallyStoppedPids.clear();
              CreateTrayMenu(true);
                UpdateTrayIcon();
                 if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    MainUIData* pData = (MainUIData*)GetWindowLongPtr(g_hMainUiWnd, GWLP_USERDATA);
                    if (pData) {
                        pData->previousAfkStartedState = false;
                        pData->stateChangeProgress = 0.0f;
                       pData->stateChangeDirection = 1;
                    }
                   InvalidateRect(g_hMainUiWnd, &pData->startButtonRect, FALSE); 

                 }
                 g_cv.notify_all();
            }
            else
            {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    MainUIData* pData = (MainUIData*)GetWindowLongPtr(g_hMainUiWnd, GWLP_USERDATA);
                    if (pData) {
                        pData->startButtonErrorText = L"Roblox not found";
                        pData->startButtonErrorTime = GetTickCount64();
                        pData->errorAnimationDirection = 1;
                    }
                }
            }
            break;
        }
        case ID_STOP_AFK:
        {
            ShowTrayNotification(L"AntiAFK Stopped", L"AntiAFK has been stopped.");
            if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
                g_totalAfkTimeSeconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
                g_afkStartTime = 0;
                SaveSettings();
            }
            g_isAfkStarted = false;
            auto wins_stop = FindAllRobloxWindows(true);
           g_manuallyStoppedPids.clear();
            for (HWND w : wins_stop) {
                DWORD pid; GetWindowThreadProcessId(w, &pid);
                if (std::find(g_manuallyStoppedPids.begin(), g_manuallyStoppedPids.end(), pid) == g_manuallyStoppedPids.end()) {
                    g_manuallyStoppedPids.push_back(pid);
                }
            }
            CreateTrayMenu(false);
            UpdateTrayIcon();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                MainUIData* pData = (MainUIData*)GetWindowLongPtr(g_hMainUiWnd, GWLP_USERDATA);
                if (pData) {
                   pData->previousAfkStartedState = true;
                    pData->stateChangeProgress = 0.0f;
                   pData->stateChangeDirection = 1;
                }
               InvalidateRect(g_hMainUiWnd, &pData->startButtonRect, FALSE);
            }
            g_cv.notify_all();
            break; 
        }

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
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            UpdateTrayIcon();
            break;
        case ID_USE_LEGACY_UI:
            g_useLegacyUi = !g_useLegacyUi.load();
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            if (!g_useLegacyUi.load())
            {
                PostMessage(hwnd, WM_COMMAND, ID_OPEN_UI, 0);
            }
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                DestroyWindow(g_hMainUiWnd);
            }
            ShowTrayNotification(L"UI Mode Changed", g_useLegacyUi.load() ? L"Switched to Legacy (Tray) mode." : L"Switched to new UI mode.");
            break;
        case ID_FISHSTRAP_SUP:
            g_fishstrapSupport = !g_fishstrapSupport.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            UpdateTrayIcon();
            break;
        case ID_AUTO_UPDATE:
            g_autoUpdate = !g_autoUpdate.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
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
        case ID_USER_SAFE_OFF:
        case ID_USER_SAFE_LEGACY:
        case ID_USER_SAFE_BETA:
            g_userSafeMode = LOWORD(wParam) - ID_USER_SAFE_OFF;
            if (g_userSafeMode > 0)
            {
                if (!g_monitorThreadRunning.load()) StartActivityMonitor();
                ShowTrayNotification(L"User-Safe Mode", L"User-Safe mode is now active.");
            }
            else
            {
                if (g_monitorThreadRunning.load()) StopActivityMonitor();
            }
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_START_AFK:
            g_autoStartAfk = !g_autoStartAfk.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_UNLOCK_FPS_ON_FOCUS:
            g_unlockFpsOnFocus = !g_unlockFpsOnFocus.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_RECONNECT:
            g_autoReconnect = !g_autoReconnect.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_BLOXSTRAP_INTEGRATION:
            g_bloxstrapIntegration = !g_bloxstrapIntegration.load();
            UpdateBloxstrapIntegration(g_bloxstrapIntegration.load());
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_RESTORE_OFF:
        case ID_RESTORE_FOREGROUND:
        case ID_RESTORE_ALTTAB:
            g_restoreMethod = LOWORD(wParam) - ID_RESTORE_OFF;
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_FPS_CAP_OFF:
        case ID_FPS_CAP_3:
        case ID_FPS_CAP_5:
        case ID_FPS_CAP_7:
        case ID_FPS_CAP_10:
        case ID_FPS_CAP_15:
        {
            int oldLimit = g_fpsLimit;
            int command = LOWORD(wParam); 

            switch (command) {
                case ID_FPS_CAP_OFF: g_fpsLimit = 0; break;
                case ID_FPS_CAP_3:   g_fpsLimit = 3; break;
                case ID_FPS_CAP_5:   g_fpsLimit = 5; break;
                case ID_FPS_CAP_7:   g_fpsLimit = 7; break;
                case ID_FPS_CAP_10:  g_fpsLimit = 10; break;
                case ID_FPS_CAP_15:  g_fpsLimit = 15; break;
            }

            if (oldLimit != g_fpsLimit) {
                if (g_isFpsCapperRunning.load()) {
                    g_isFpsCapperRunning = false;
                    if (g_fpsCapperThread.joinable()) {
                        g_fpsCapperThread.join();
                    }
                }
                if (g_fpsLimit > 0) { g_fpsCapperThread = std::thread(FpsCapperThread); } SaveSettings(); CreateTrayMenu(g_isAfkStarted.load()); 
            }
            break;
        }
        case ID_FPS_CAP_CUSTOM:
        {
            ShowCustomFpsDialog(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        }
        case ID_RESET_SETTINGS:
        {
            int result = ShowDarkMessageBox(hwnd, L"Are you sure you want to reset all settings to their defaults?\nThis action cannot be undone.", L"Reset Settings", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            if (result == IDYES)
            {
                ResetSettings();
                CreateTrayMenu(g_isAfkStarted.load());
                UpdateTrayIcon();
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
                ShowTrayNotification(L"Settings Reset", L"All settings have been restored to their defaults.");
            }
            break;
        }
        case ID_TIME_CUSTOM:
        ShowCustomIntervalDialog(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
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
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
                CreateTrayMenu(g_isAfkStarted.load());
            }
            break;
        }
        case ID_ACTION_SPACE:
        case ID_ACTION_WS:
        case ID_ACTION_ZOOM:
            g_selectedAction = LOWORD(wParam) - ID_ACTION_SPACE;
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_EXIT:
            ShowAllRobloxWindows_Multi();
            if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
                g_totalAfkTimeSeconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
            }
            SaveSettings();
            g_stopThread = true;
            g_cv.notify_all();
            Shell_NotifyIcon(NIM_DELETE, &g_nid);
            PostQuitMessage(0);
            break;
        default:
            if (LOWORD(wParam) >= ID_FPS_CAP_CUSTOM_BASE && LOWORD(wParam) <= ID_FPS_CAP_CUSTOM_BASE + 60) {
                int oldLimit = g_fpsLimit;
                g_fpsLimit = LOWORD(wParam) - ID_FPS_CAP_CUSTOM_BASE;
                if (oldLimit != g_fpsLimit) {
                    if (g_isFpsCapperRunning.load()) { g_isFpsCapperRunning = false; if (g_fpsCapperThread.joinable()) g_fpsCapperThread.join(); } if (g_fpsLimit > 0) { g_fpsCapperThread = std::thread(FpsCapperThread); } SaveSettings(); CreateTrayMenu(g_isAfkStarted.load()); 
                }
            }
            break;
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &g_nid);
        if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
            g_totalAfkTimeSeconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
        }
        SaveSettings();
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
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    g_hInst = hInstance;
    const wchar_t CLASS_NAME[] = L"AntiAFK-RBX-tray";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    
    LoadSettings();

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    bool arg_noSplash = false, arg_tray = false, arg_startAfk = false;
    bool arg_checkUpdatesAndExit = false, arg_resetSettingsAndExit = false;
    bool arg_force = false;
    bool arg_bloxstrapIntegration_cmd = false;

    if (argv) {
        for (int i = 1; i < argc; ++i) {
            std::wstring arg = argv[i];

            if (arg == L"--force") arg_force = true;
            else if (arg == L"--no-splash") arg_noSplash = true;
            else if (arg == L"--tray" || arg == L"--no-ui") arg_tray = true;
            else if (arg == L"--start-afk") arg_startAfk = true;
            else if (arg == L"--legacy-ui") g_useLegacyUi = true;
            else if (arg == L"--no-notifications") g_notificationsDisabled = true;
            else if (arg == L"--auto-update" || arg == L"--no-update-check") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_autoUpdate = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_autoUpdate = (arg != L"--no-update-check");
                }
            }
            else if (arg == L"--help" || arg == L"-?") {
                ShowHelp();
                LocalFree(argv);
                return 0;
            }
            else if (arg == L"--check-updates") arg_checkUpdatesAndExit = true;
            else if (arg == L"--reset-settings") arg_resetSettingsAndExit = true;
            else if (arg == L"--set-interval" && i + 1 < argc) {
                try {
                    int val = std::stoi(argv[++i]);
                    if (val > 0) g_selectedTime = val;
                } catch (...) {}
            }
            else if (arg == L"--set-action" && i + 1 < argc) {
                std::wstring val = argv[++i];
                if (val == L"space") g_selectedAction = 0;
                else if (val == L"ws") g_selectedAction = 1;
                else if (val == L"zoom") g_selectedAction = 2;
            }
            else if (arg == L"--multi-instance") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_multiSupport = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_multiSupport = true;
                }
            }
            else if (arg == L"--auto-start") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_autoStartAfk = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_autoStartAfk = true;
                }
            }
            else if (arg == L"--auto-reconnect") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_autoReconnect = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_autoReconnect = true;
                }
            }
            else if (arg == L"--user-safe" && i + 1 < argc) {
                std::wstring val = argv[++i];
                if (val == L"off") g_userSafeMode = 0;
                else if (val == L"legacy") g_userSafeMode = 1;
                else if (val == L"beta") g_userSafeMode = 2;
            }
            else if (arg == L"--restore-window" && i + 1 < argc) {
                std::wstring val = argv[++i];
                if (val == L"off") g_restoreMethod = 0;
                else if (val == L"foreground") g_restoreMethod = 1;
                else if (val == L"alttab") g_restoreMethod = 2;
            }
            else if (arg == L"--fishstrap-support") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_fishstrapSupport = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_fishstrapSupport = true;
                }
            }
            else if (arg == L"--bloxstrap-integration") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_bloxstrapIntegration = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_bloxstrapIntegration = true;
                }
                arg_bloxstrapIntegration_cmd = true;
            }
            else if (arg == L"--unlock-fps-on-focus") {
                if (i + 1 < argc && (std::wstring(argv[i + 1]) == L"on" || std::wstring(argv[i + 1]) == L"off")) {
                    g_unlockFpsOnFocus = (std::wstring(argv[++i]) == L"on");
                } else {
                    g_unlockFpsOnFocus = true;
                }
            }
            else if (arg == L"--set-fps-limit" && i + 1 < argc) {
                try {
                    int val = std::stoi(argv[++i]);
                    if (val >= 0 && val <= 60) {
                        g_fpsLimit = val;
                    }
                }
                catch (...) {}
            }
        }
        LocalFree(argv);
    }
    
    if (arg_bloxstrapIntegration_cmd) {
        arg_noSplash = true;
        arg_tray = true;
        g_notificationsDisabled = true;
        arg_startAfk = true;
    }

    HWND hExistingWnd = FindWindow(CLASS_NAME, NULL);
    if (hExistingWnd) {
        if (arg_force) {
            DWORD dwProcessId = 0;
            GetWindowThreadProcessId(hExistingWnd, &dwProcessId);
            if (dwProcessId != 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    Sleep(500);
                }
            }
        } else if (arg_bloxstrapIntegration_cmd) {
            return 0;
        } else {
            ShowDarkMessageBox(NULL, L"AntiAFK-RBX is already running.\nCheck the system tray or use --force to restart.", L"AntiAFK-RBX", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
    }

    if (arg_resetSettingsAndExit) {
        int result = ShowDarkMessageBox(NULL, L"Are you sure you want to reset all settings to their defaults via command line?\nThis action cannot be undone.", L"Reset Settings", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
        if (result == IDYES) {
            ResetSettings();
            ShowDarkMessageBox(NULL, L"All settings have been restored to their defaults.", L"Settings Reset", MB_OK | MB_ICONINFORMATION);
        }
        return 0;
    }

    if (arg_checkUpdatesAndExit) {
        CheckForUpdates(false);
        ShowDarkMessageBox(NULL, g_updateFound ? L"A new version is available." : L"You are using the latest version.", L"Update Check", MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    if (arg_startAfk) {
        g_autoStartAfk = false;
        g_isAfkStarted = true;
    }

    if (!arg_noSplash) {
        CreateSplashScreen(hInstance);
    }

    RegisterClass(&wc);
    g_hwnd = CreateWindowEx(0, CLASS_NAME, L"AntiAFK-RBX-tray", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (g_multiSupport.load())
        EnableMultiInstanceSupport();
    ShowWindow(g_hwnd, SW_HIDE);

    if (arg_tray) {
        g_tutorialShown = true; 
    }
    if (arg_startAfk) {
        ShowTrayNotification(L"AntiAFK-RBX | Started via Argument", L"AntiAFK has started automatically.");
    }

    std::thread t(main_thread, arg_tray); 
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

    if (g_isFpsCapperRunning.load()) {
        g_isFpsCapperRunning = false;
        if (g_fpsCapperThread.joinable()) g_fpsCapperThread.join();
    }

    DisableMultiInstanceSupport();
    StopActivityMonitor();

    GdiplusShutdown(gdiplusToken);
    return 0;
}
// ==========