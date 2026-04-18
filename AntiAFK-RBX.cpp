// AntiAFK-RBX.cpp | The best program for AntiAFK and Multi-Instance in Roblox. Or just Roblox Anti-AFK. | By Agzes
// https://github.com/Agzes/AntiAFK-RBX • \[=_=]/


int currentVersion = 30200; // Major*10000 + Minor*100 + Patch or Mini Update
const wchar_t* g_Version = L"v.3.2.0";

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <strsafe.h>
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
#include <map>
#include <cstring>
#include <cwctype>
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
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "WinInet.lib")
#pragma comment(lib, "Comdlg32.lib")


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
#define ID_GRID_SNAP 9
#define ID_WINDOW_OPACITY 10

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
#define ID_IMPORT_SETTINGS 320
#define ID_EXPORT_SETTINGS 321

#define ID_USE_LEGACY_UI 310
#define ID_BLOXSTRAP_INTEGRATION 311
#define ID_RESTORE_METHOD_SUBMENU 400
#define ID_UNLOCK_FPS_ON_FOCUS 312
#define ID_AUTO_RESET 313
#define ID_AUTO_HIDE 314
#define ID_I_CAN_FORGET 315
#define ID_STATUS_BAR 316
#define ID_AUTO_OPACITY 317
#define ID_AUTO_GRID 318

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

#define ID_MI_INTERVAL_SUBMENU 700
#define ID_MI_INTERVAL_0 702
#define ID_MI_INTERVAL_1 703
#define ID_MI_INTERVAL_3 704
#define ID_MI_INTERVAL_5 705
#define ID_MI_INTERVAL_10 706

#define ID_UPDATE_AVAILABLE 1000
#define ID_ANNOUNCEMENT_TEXT 1001

#define ID_EDIT_SECONDS 1100
#define ID_BTN_GITHUB 1101
#define ID_BTN_CLOSE_ABOUT 1102
#define ID_BTN_NEXT 1201
#define ID_BTN_BACK 1202
#define ID_BTN_SKIP 1203
#define ID_OPEN_UI 1301
#define ID_DO_NOT_SLEEP 1302
#define ID_AUTO_MUTE 1303
#define ID_UNMUTE_ON_FOCUS 1304
#define ID_UTILS_CLOSE_ALL 1305
#define ID_UTILS_RESET_ALL 1306
#define ID_UTILS_TEST_ACTION 1307
#define ID_UTILS_TOGGLE_MUTE 1308
#define ID_UTILS_TOGGLE_FPS 1309
#define ID_UTILS_SHOW_ALL 1310
#define ID_UTILS_HIDE_ALL 1311
#define ID_UTILS_TOGGLE_WINDOW_OPACITY 1312
#define ID_RESET_STATS 1313
#define ID_DISCORD_WEBHOOK_ENABLE 1314
#define ID_DISCORD_WEBHOOK_PASTE 1315
#define ID_DISCORD_WEBHOOK_CLEAR 1316
#define ID_DISCORD_WEBHOOK_TEST 1317
#define ID_DISCORD_NOTIFY_START 1318
#define ID_DISCORD_NOTIFY_ACTION 1319
#define ID_DISCORD_NOTIFY_RECONNECT 1320
#define ID_DISCORD_NOTIFY_ERRORS 1321
#define ID_DISCORD_DISABLE_EMBED 1322
#define ID_DISCORD_MENTION_ON_ERRORS 1323

constexpr UINT WM_APP_SHOW_STATUS_BAR = WM_APP + 20;
constexpr UINT STATUS_BAR_HIDE_TIMER = 1;
constexpr UINT STATUS_BAR_ANIM_TIMER = 2;
constexpr UINT STATUS_BAR_DEFAULT_DURATION = 1800;
constexpr UINT STATUS_BAR_PRE_ACTION_DELAY = 2000;
constexpr UINT STATUS_BAR_POST_ACTION_DURATION = 1000;
constexpr DWORD FPS_CAPPER_PRE_ACTION_PAUSE_MS = 1500;
constexpr UINT STATUS_BAR_ACTION_PENDING_DURATION = 120000;


HWND g_hwnd;
HINSTANCE g_hInst;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
HANDLE g_hMultiInstanceMutex = NULL;
HWND g_hSplashWnd = NULL;
HWND g_hStatusBarWnd = NULL;
HWND g_hMainUiWnd = NULL;
HWND g_hCustomFpsWnd = NULL;
HWND g_hCustomIntervalWnd = NULL;
std::atomic<bool> g_isAfkStarted(false), g_stopThread(false), g_multiSupport(false), g_fishstrapSupport(false), g_autoUpdate(true), g_updateFound(false), g_updateCheckFailed(false), g_autoStartAfk(false), g_autoReconnect(false), g_autoReset(false), g_autoHideRoblox(false), g_autoOpacity(false), g_autoGrid(false), g_userActive(false), g_monitorThreadRunning(false), g_updateInterval(false), g_tutorialShown(false), g_useLegacyUi(false), g_statusBarEnabled(true), g_unlockFpsOnFocus(false), g_notificationsDisabled(false), g_bloxstrapIntegration(false), g_isFpsCapperRunning(false),  g_isFpsCapperPaused(false), g_windowOpacity(false), g_afkReminderEnabled(false), g_doNotSleep(false), g_autoMute(false), g_unmuteOnFocus(false);
std::atomic<bool> g_discordWebhookEnabled(false), g_discordNotifyStart(true), g_discordNotifyStop(true), g_discordNotifyAction(false), g_discordNotifyReconnect(true), g_discordNotifyReset(false), g_discordNotifyErrors(true), g_discordDisableEmbed(false), g_discordMentionOnErrors(false);
std::atomic<int> g_afkReminderState(0); // 0 - no reminder, 1 - reminded at 18min, 2 - AFK started at 19min
std::atomic<uint64_t> g_lastActivityTime(0), g_afkStartTime(0), g_lastAfkActionTimestamp(0), g_autoReconnectsPerformed(0), g_afkActionsPerformed(0), g_totalAfkTimeSeconds(0), g_longestAfkSessionSeconds(0), g_discordWebhooksSent(0), g_programLaunches(0), g_afkSessionsCompleted(0);
std::atomic<DWORD> g_unmutedPid(0);
std::thread g_activityMonitorThread;
std::thread g_fpsCapperThread;
std::condition_variable g_cv;
std::vector<DWORD> g_manuallyStoppedPids;
std::mutex g_autoWindowLayoutMutex;
std::vector<UINT_PTR> g_lastAutoGridWindowSignature;
std::mutex g_cv_m;
std::mutex g_discordWebhookMutex;
std::wstring g_discordWebhookUrl;

const TCHAR g_szClassName[] = _T("AntiAFK-RBX-tray");
wchar_t g_splashStatus[128] = L"Initializing...";
constexpr DWORD ACTION_DELAY = 30, ALT_DELAY = 15;
int g_selectedTime = 540;
int g_selectedAction = 0; // 0 - space, 1 - w&s, 2 - zoom
int g_restoreMethod = 1; // 0 - Off, 1 - SetForeground, 2 - Alt+Tab
int g_userSafeMode = 0; // 0 - Off, 1 - Legacy, 2 - Beta
int g_fpsLimit = 0; // 0 for OFF
int g_multiInstanceInterval = 0; 
std::atomic<int> g_utilsWindowOpacityOverride(-1); // -1 = use saved setting, 0 = force off, 1 = force on
std::atomic<int> g_utilsMuteOverride(-1); // -1 = use normal behavior, 0 = force unmute, 1 = force mute
std::atomic<int> g_utilsFpsLimitOverride(-1); // -1 = use saved setting, >0 = force limit
const int USER_INACTIVITY_WAIT = 3;
const int MAX_WAIT_TIME = 60;
const int AFK_REMINDER_TIME = 18 * 60;
const int AFK_AUTO_START_TIME = 19 * 60;
const size_t DISCORD_WEBHOOK_MAX_LENGTH = 2048;
const COLORREF DARK_BG = RGB(30, 30, 30);
const COLORREF DARK_TEXT = RGB(235, 235, 235);
const COLORREF DARK_EDIT_BG = RGB(45, 45, 45);

struct StatusBarPayload {
    std::wstring text;
    UINT durationMs;
    HWND anchorWindow;
};


bool CheckForUpdates(bool showNotification);
void SaveSettings();
std::string EscapeJsonStringUtf8(const std::string& input);
void FpsCapperThread();
void ShowAllRobloxWindows_Multi();
void RefreshRobloxWindowOpacity(bool forceDisable);
void RestoreForegroundWindow(HWND prevWnd);
bool PauseFpsCapperBeforeAction(DWORD waitMs = FPS_CAPPER_PRE_ACTION_PAUSE_MS, bool allowAbort = false);
void ResumeFpsCapperAfterAction(bool previousPausedState);
enum class DiscordWebhookEvent
{
    Test,
    Started,
    Stopped,
    Action,
    AutoReconnect,
    AutoReset,
    Error
};
void QueueDiscordWebhookEvent(DiscordWebhookEvent eventType, const std::wstring& summary, bool bypassEnabledCheck = false);
uint64_t FinalizeAfkSession();
std::wstring FormatDurationShort(uint64_t totalSeconds);
std::wstring GetDiscordWebhookUrlCopy();
bool IsDiscordWebhookUrl(const std::wstring& url);
void QueueStatusBarOverlay(const std::wstring& message, UINT durationMs = STATUS_BAR_DEFAULT_DURATION, HWND anchorWindow = NULL);
void ShowStatusBarOverlay(const std::wstring& message, UINT durationMs = STATUS_BAR_DEFAULT_DURATION, HWND anchorWindow = NULL);
bool TryParseOnOffValue(const std::wstring& value, bool& outValue);
bool ReadOptionalOnOffArgument(LPWSTR* argv, int argc, int& index, bool defaultValue, bool& outValue);
void ResetStatisticsCounters();

// Args
void ShowHelp()
{
    bool attachedToParentConsole = AttachConsole(ATTACH_PARENT_PROCESS) != FALSE;
    bool allocatedConsole = false;
    if (!attachedToParentConsole)
    {
        allocatedConsole = AllocConsole() != FALSE;
    }

    if (attachedToParentConsole || allocatedConsole)
    {
        const wchar_t* helpText = L""
            L"\r\n\r\n"
            L"AntiAFK-RBX - Command Line Help\r\n"
            L"================================================\r\n\r\n"
            L"Usage: AntiAFK-RBX.exe [options]\r\n\r\n"
            L"Startup & UI:\r\n"
            L"  --no-splash\r\n"
            L"      Skip the splash screen on startup.\r\n"
            L"  --tray\r\n"
            L"      Start minimized to the system tray without showing any UI.\r\n"
            L"  --legacy-ui [on|off]\r\n"
            L"      Force enable or disable the tray-only Legacy UI.\r\n"
            L"  --status-bar [on|off]\r\n"
            L"      Force enable or disable the Status Bar overlay.\r\n"
            L"  --no-notifications\r\n"
            L"      Disable all non-critical tray notifications.\r\n"
            L"  --auto-update [on|off]\r\n"
            L"      Force enable or disable update checking.\r\n"
            L"  --no-update-check\r\n"
            L"      Skip checking for updates on startup.\r\n"
            L"  --help, -?\r\n"
            L"      Show this help message and exit.\r\n\r\n"

            L"Anti-AFK Control:\r\n"
            L"  --start-afk\r\n"
            L"      Start the Anti-AFK function immediately on launch.\r\n"
            L"  --set-interval <seconds>\r\n"
            L"      Set the AFK interval in seconds (example: --set-interval 300).\r\n"
            L"  --set-action <action>\r\n"
            L"      Set the AFK action. Values: space, ws, zoom.\r\n\r\n"

            L"Settings Override (current session only):\r\n"
            L"  --multi-instance [on|off]\r\n"
            L"      Force enable or disable multi-instance support.\r\n"
            L"  --multi-instance-interval <0|1|3|5|10>\r\n"
            L"      Set delay between Roblox windows in seconds.\r\n"
            L"  --auto-start [on|off]\r\n"
            L"      Force enable or disable the Auto-Start feature.\r\n"
            L"  --auto-reconnect [on|off]\r\n"
            L"      Force enable or disable the Auto Reconnect feature.\r\n"
            L"  --auto-reset [on|off]\r\n"
            L"      Force enable or disable the Auto Reset feature.\r\n"
            L"  --auto-hide [on|off]\r\n"
            L"      Force enable or disable auto-hiding Roblox.\r\n"
            L"  --auto-opacity [on|off]\r\n"
            L"      Force enable or disable auto opacity.\r\n"
            L"  --auto-grid [on|off]\r\n"
            L"      Force enable or disable auto grid.\r\n"
            L"  --user-safe <mode>\r\n"
            L"      Set User-Safe mode. Values: off, legacy, beta.\r\n"
            L"  --restore-window <method>\r\n"
            L"      Set restore method. Values: off, foreground, alttab.\r\n"
            L"  --fishstrap-support [on|off]\r\n"
            L"      Force enable or disable support for modified clients.\r\n"
            L"  --bloxstrap-integration [on|off]\r\n"
            L"      Force enable or disable Fish/Void/Bloxstrap integration.\r\n"
            L"  --do-not-sleep [on|off]\r\n"
            L"      Force enable or disable sleep prevention.\r\n"
            L"  --unlock-fps-on-focus [on|off]\r\n"
            L"      Force enable or disable unlocking FPS when Roblox is focused.\r\n"
            L"  --set-fps-limit <fps>\r\n"
            L"      Set the FPS limit (0 to disable).\r\n"
            L"  --auto-mute [on|off]\r\n"
            L"      Force enable or disable automatic Roblox mute.\r\n"
            L"  --unmute-on-focus [on|off]\r\n"
            L"      Force enable or disable temporary unmute on focus.\r\n\r\n"

            L"Discord Webhook:\r\n"
            L"  --discord-webhook [on|off]\r\n"
            L"      Force enable or disable webhook sending.\r\n"
            L"  --discord-webhook-url <url>\r\n"
            L"      Set the Discord webhook URL for this session.\r\n"
            L"  --discord-notify-start [on|off]\r\n"
            L"      Force enable or disable Start / Stop webhook events.\r\n"
            L"  --discord-notify-action [on|off]\r\n"
            L"      Force enable or disable Action webhook events.\r\n"
            L"  --discord-notify-reconnect [on|off]\r\n"
            L"      Force enable or disable Reconnect webhook events.\r\n"
            L"  --discord-notify-errors [on|off]\r\n"
            L"      Force enable or disable Error webhook events.\r\n"
            L"  --discord-disable-embed [on|off]\r\n"
            L"      Force enable or disable embed payloads.\r\n"
            L"  --discord-mention-on-errors [on|off]\r\n"
            L"      Force enable or disable @everyone mentions on errors.\r\n\r\n"

            L"Utils:\r\n"
            L"  --utils-show-all\r\n"
            L"      Show all Roblox windows.\r\n"
            L"  --utils-hide-all\r\n"
            L"      Hide all Roblox windows.\r\n"
            L"  --utils-grid-snap\r\n"
            L"      Arrange Roblox windows into the grid layout.\r\n"
            L"  --utils-window-opacity [on|off]\r\n"
            L"      Set the session-only Roblox opacity override.\r\n"
            L"  --utils-mute [on|off]\r\n"
            L"      Set the session-only Roblox mute override.\r\n"
            L"  --utils-fps [on|off]\r\n"
            L"      Set the session-only FPS override.\r\n"
            L"  --utils-test-action\r\n"
            L"      Run the manual AntiAFK test action on Roblox.\r\n"
            L"  --utils-reset-all\r\n"
            L"      Send Reset to all detected Roblox windows.\r\n"
            L"  --utils-close-all\r\n"
            L"      Close all detected Roblox clients.\r\n\r\n"

            L"Notes:\r\n"
            L"  Features marked with * in the UI are experimental.\r\n"
            L"  Auto Reconnect*: may occasionally fail to click the reconnect button.\r\n"
            L"  Auto Grid*: in rare cases can mess up Roblox window positions.\r\n"
            L"  Grid Snap Roblox*: in rare cases can also misplace window positions.\r\n"
            L"  FPS Capper*: limits process timing rather than exact FPS.\r\n"
            L"  Status Bar*: experimental UI overlay.\r\n\r\n"
            L"================================================\r\n\r\n";

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE && hOut != NULL)
        {
            DWORD charsWritten = 0;
            WriteConsoleW(hOut, helpText, (DWORD)wcslen(helpText), &charsWritten, NULL);
        }

        if (attachedToParentConsole || allocatedConsole)
        {
            FreeConsole();
        }
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
    StringCchCopyW(info.szInfoTitle, _countof(info.szInfoTitle), title ? title : L"");
    StringCchCopyW(info.szInfo, _countof(info.szInfo), msg ? msg : L"");
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
static void Popup_DrawActionButton(Graphics* g, HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool isHovering, bool isPrimary) {
    Color fillColor = isPrimary
        ? (isHovering ? Color(170, 20, 142, 224) : Color(140, 0, 122, 204))
        : (isHovering ? Color(70, 60, 60, 60) : Color(50, 50, 50, 50));
    Color borderColor = isPrimary ? Color(180, 0, 122, 204) : Color(80, 70, 70, 70);

    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);
    g->FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawRectangle(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left - 1), (REAL)(rect.bottom - rect.top - 1));

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF rectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawString(text, -1, &textFont, rectF, &sf, &textBrush);
}
static void Popup_DrawActionButtonWithoutTopBorder(Graphics* g, HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool isHovering, bool isPrimary) {
    Color fillColor = isPrimary
        ? (isHovering ? Color(120, 20, 142, 224) : Color(90, 0, 122, 204))
        : (isHovering ? Color(70, 60, 60, 60) : Color(50, 50, 50, 50));
    Color borderColor = isPrimary ? Color(180, 0, 122, 204) : Color(80, 70, 70, 70);

    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);
    g->FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawLine(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)rect.left, (REAL)(rect.bottom - 1));
    g->DrawLine(&borderPen, (REAL)(rect.right - 1), (REAL)rect.top, (REAL)(rect.right - 1), (REAL)(rect.bottom - 1));
    g->DrawLine(&borderPen, (REAL)rect.left, (REAL)(rect.bottom - 1), (REAL)(rect.right - 1), (REAL)(rect.bottom - 1));

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF rectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawString(text, -1, &textFont, rectF, &sf, &textBrush);
}
static void Popup_DrawTextInput(Graphics* g, HDC hdc, const RECT& rect, HFONT inputFont, const wchar_t* value, const wchar_t* placeholder, bool isHovering, bool isFocused) {
    Color fillColor = isFocused ? Color(140, 55, 55, 55) : (isHovering ? Color(130, 50, 50, 50) : Color(120, 45, 45, 45));
    Color borderColor = isFocused ? Color(180, 0, 122, 204) : Color(180, 80, 80, 80);
    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);

    g->FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawRectangle(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left - 1), (REAL)(rect.bottom - rect.top - 1));

    Font textFont(hdc, inputFont);
    SolidBrush textBrush((value && value[0] != L'\0') ? Color(255, 255, 255, 255) : Color(255, 130, 130, 130));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    RectF textRect((REAL)(rect.left + 12), (REAL)rect.top, (REAL)(rect.right - rect.left - 24), (REAL)(rect.bottom - rect.top));
    g->DrawString((value && value[0] != L'\0') ? value : placeholder, -1, &textFont, textRect, &sf, &textBrush);
}
static void Popup_DrawTextInputJoinedToButtons(Graphics* g, HDC hdc, const RECT& rect, HFONT inputFont, const wchar_t* value, const wchar_t* placeholder, bool isHovering, bool isFocused) {
    Color fillColor = isFocused ? Color(140, 55, 55, 55) : (isHovering ? Color(130, 50, 50, 50) : Color(120, 45, 45, 45));
    Color borderColor = isFocused ? Color(180, 0, 122, 204) : Color(180, 80, 80, 80);
    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);

    g->FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawLine(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - 1), (REAL)rect.top);
    g->DrawLine(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)rect.left, (REAL)(rect.bottom - 1));
    g->DrawLine(&borderPen, (REAL)(rect.right - 1), (REAL)rect.top, (REAL)(rect.right - 1), (REAL)(rect.bottom - 1));
    g->DrawLine(&borderPen, (REAL)rect.left, (REAL)(rect.bottom - 1), (REAL)(rect.right - 1), (REAL)(rect.bottom - 1));

    Font textFont(hdc, inputFont);
    SolidBrush textBrush((value && value[0] != L'\0') ? Color(255, 255, 255, 255) : Color(255, 130, 130, 130));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    RectF textRect((REAL)(rect.left + 12), (REAL)rect.top, (REAL)(rect.right - rect.left - 24), (REAL)(rect.bottom - rect.top));
    g->DrawString((value && value[0] != L'\0') ? value : placeholder, -1, &textFont, textRect, &sf, &textBrush);
}
static void Popup_DrawInfoBanner(Graphics* g, HDC hdc, const RECT& rect, HFONT font, const wchar_t* icon, COLORREF iconColor, const wchar_t* text) {
    SolidBrush infoBgBrush(Color(60, 45, 45, 45));
    Pen infoBorderPen(Color(180, 80, 80, 80), 1.0f);
    g->FillRectangle(&infoBgBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawRectangle(&infoBorderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left - 1), (REAL)(rect.bottom - rect.top - 1));

    HFONT iconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
    Font iconFont(hdc, iconFontH);
    SolidBrush iconBrush(Color(255, GetRValue(iconColor), GetGValue(iconColor), GetBValue(iconColor)));
    RectF iconRect((REAL)(rect.left + 6), (REAL)rect.top, 20.0f, (REAL)(rect.bottom - rect.top));
    StringFormat sfIcon;
    sfIcon.SetAlignment(StringAlignmentCenter);
    sfIcon.SetLineAlignment(StringAlignmentCenter);
    g->DrawString(icon, -1, &iconFont, iconRect, &sfIcon, &iconBrush);
    DeleteObject(iconFontH);

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 140, 140, 140));
    RectF textRect((REAL)(rect.left + 30), (REAL)rect.top, (REAL)(rect.right - rect.left - 38), (REAL)(rect.bottom - rect.top));
    StringFormat sfText;
    sfText.SetAlignment(StringAlignmentNear);
    sfText.SetLineAlignment(StringAlignmentCenter);
    g->DrawString(text, -1, &textFont, textRect, &sfText, &textBrush);
}
static void Popup_DrawInfoBannerNoBottomBorder(Graphics* g, HDC hdc, const RECT& rect, HFONT font, const wchar_t* icon, COLORREF iconColor, const wchar_t* text) {
    SolidBrush infoBgBrush(Color(60, 45, 45, 45));
    Pen infoBorderPen(Color(180, 80, 80, 80), 1.0f);
    g->FillRectangle(&infoBgBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g->DrawLine(&infoBorderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - 1), (REAL)rect.top);
    g->DrawLine(&infoBorderPen, (REAL)rect.left, (REAL)rect.top, (REAL)rect.left, (REAL)(rect.bottom - 1));
    g->DrawLine(&infoBorderPen, (REAL)(rect.right - 1), (REAL)rect.top, (REAL)(rect.right - 1), (REAL)(rect.bottom - 1));

    HFONT iconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
    Font iconFont(hdc, iconFontH);
    SolidBrush iconBrush(Color(255, GetRValue(iconColor), GetGValue(iconColor), GetBValue(iconColor)));
    RectF iconRect((REAL)(rect.left + 6), (REAL)rect.top, 20.0f, (REAL)(rect.bottom - rect.top));
    StringFormat sfIcon;
    sfIcon.SetAlignment(StringAlignmentCenter);
    sfIcon.SetLineAlignment(StringAlignmentCenter);
    g->DrawString(icon, -1, &iconFont, iconRect, &sfIcon, &iconBrush);
    DeleteObject(iconFontH);

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 140, 140, 140));
    RectF textRect((REAL)(rect.left + 30), (REAL)rect.top, (REAL)(rect.right - rect.left - 38), (REAL)(rect.bottom - rect.top));
    StringFormat sfText;
    sfText.SetAlignment(StringAlignmentNear);
    sfText.SetLineAlignment(StringAlignmentCenter);
    g->DrawString(text, -1, &textFont, textRect, &sfText, &textBrush);
}
static void Popup_DrawChrome(Graphics* g, HDC hdc, const RECT& clientRect, const RECT& closeButtonRect, HFONT titleFont, const wchar_t* title, bool isHoveringClose) {
    SolidBrush bgBrush(Color(100, 10, 10, 10));
    g->FillRectangle(&bgBrush, (REAL)clientRect.left, (REAL)clientRect.top, (REAL)(clientRect.right - clientRect.left), (REAL)(clientRect.bottom - clientRect.top));

    Pen headerLinePen(Color(40, 255, 255, 255), 1.0f);
    g->DrawLine(&headerLinePen, 0.0f, 30.0f, (REAL)clientRect.right, 30.0f);

    HICON hAppIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_OFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    if (hAppIcon) {
        DrawIconEx(hdc, 15, 7, hAppIcon, 16, 16, 0, NULL, DI_NORMAL);
        DestroyIcon(hAppIcon);
    }

    Font popupTitleFont(hdc, titleFont);
    SolidBrush titleBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF titleRect(0.0f, 0.0f, (REAL)clientRect.right, 30.0f);
    g->DrawString(title, -1, &popupTitleFont, titleRect, &sf, &titleBrush);

    DrawSharedCloseButton(g, closeButtonRect, isHoveringClose);
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
    int updateCheckState = 0; // 0: idle, 1: checking, 2: found, 3: not found, 4: error
    COLORREF updateButtonCurrentColor = RGB(160, 160, 160);
    UINT_PTR uTimerId = 0;
};

void About_Paint_DrawCloseButton(HDC hdc, const RECT& closeButtonRect, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    DrawSharedCloseButton(&g, closeButtonRect, isHovering);
}
void About_Paint_DrawButtons(HDC hdc, const RECT& githubButtonRect, const RECT& sourceforgeButtonRect, bool isHoveringGithub, bool isHoveringSourceforge, HFONT hFont) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RECT groupRect = { githubButtonRect.left, githubButtonRect.top, sourceforgeButtonRect.right, githubButtonRect.bottom };
    Color groupBorderColor(80, 70, 70, 70);
    Color githubFill = isHoveringGithub ? Color(70, 60, 60, 60) : Color(50, 50, 50, 50);
    Color sourceforgeFill = isHoveringSourceforge ? Color(70, 60, 60, 60) : Color(50, 50, 50, 50);

    SolidBrush githubBrush(githubFill);
    SolidBrush sourceforgeBrush(sourceforgeFill);
    Pen borderPen(groupBorderColor, 1.0f);

    g.FillRectangle(&githubBrush, (REAL)githubButtonRect.left, (REAL)githubButtonRect.top, (REAL)(githubButtonRect.right - githubButtonRect.left), (REAL)(githubButtonRect.bottom - githubButtonRect.top));
    g.FillRectangle(&sourceforgeBrush, (REAL)sourceforgeButtonRect.left, (REAL)sourceforgeButtonRect.top, (REAL)(sourceforgeButtonRect.right - sourceforgeButtonRect.left), (REAL)(sourceforgeButtonRect.bottom - sourceforgeButtonRect.top));

    g.DrawLine(&borderPen, (REAL)groupRect.left, (REAL)groupRect.top, (REAL)groupRect.left, (REAL)(groupRect.bottom - 1));
    g.DrawLine(&borderPen, (REAL)(groupRect.right - 1), (REAL)groupRect.top, (REAL)(groupRect.right - 1), (REAL)(groupRect.bottom - 1));
    g.DrawLine(&borderPen, (REAL)groupRect.left, (REAL)(groupRect.bottom - 1), (REAL)(groupRect.right - 1), (REAL)(groupRect.bottom - 1));
    g.DrawLine(&borderPen, (REAL)sourceforgeButtonRect.left, (REAL)groupRect.top, (REAL)sourceforgeButtonRect.left, (REAL)(groupRect.bottom - 1));

    Font textFont(hdc, hFont);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    RectF githubRectF((REAL)githubButtonRect.left, (REAL)githubButtonRect.top, (REAL)(githubButtonRect.right - githubButtonRect.left), (REAL)(githubButtonRect.bottom - githubButtonRect.top));
    RectF sourceforgeRectF((REAL)sourceforgeButtonRect.left, (REAL)sourceforgeButtonRect.top, (REAL)(sourceforgeButtonRect.right - sourceforgeButtonRect.left), (REAL)(sourceforgeButtonRect.bottom - sourceforgeButtonRect.top));
    g.DrawString(L"GitHub", -1, &textFont, githubRectF, &sf, &textBrush);
    g.DrawString(L"SourceForge", -1, &textFont, sourceforgeRectF, &sf, &textBrush);
}
void About_Paint_DrawUpdateLine(HDC hdc, const RECT& updateRect, HFONT hFont, const std::wstring& text, COLORREF color, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    if (isHovering) {
        SolidBrush hoverBrush(Color(35, 55, 55, 55));
        g.FillRectangle(&hoverBrush, (REAL)updateRect.left, (REAL)updateRect.top, (REAL)(updateRect.right - updateRect.left), (REAL)(updateRect.bottom - updateRect.top));
    }

    Pen linePen(Color(100, 80, 80, 80), 1.0f);
    g.DrawLine(&linePen, (REAL)updateRect.left, (REAL)updateRect.top, (REAL)updateRect.right, (REAL)updateRect.top);
    g.DrawLine(&linePen, (REAL)updateRect.left, (REAL)(updateRect.bottom - 1), (REAL)updateRect.right, (REAL)(updateRect.bottom - 1));

    Font font(hdc, hFont);
    SolidBrush textBrush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF rectF((REAL)updateRect.left, (REAL)updateRect.top, (REAL)(updateRect.right - updateRect.left), (REAL)(updateRect.bottom - updateRect.top));
    g.DrawString(text.c_str(), -1, &font, rectF, &sf, &textBrush);
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
    case 4: targetColor = RGB(210, 110, 90); break;
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
        const int btnHeight = 36, btnGap = 0, margin = 0, updateHeight = 22;
        int btnY = clientRect.bottom - btnHeight;
        int halfWidth = (clientRect.right - btnGap) / 2;
        pData->githubButtonRect = { 0, btnY, halfWidth, btnY + btnHeight };
        pData->sourceforgeButtonRect = { pData->githubButtonRect.right + btnGap, btnY, clientRect.right, btnY + btnHeight };

        int updateBtnY = btnY - updateHeight;
        pData->updateButtonRect = { 0, updateBtnY, clientRect.right, updateBtnY + updateHeight };
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
            pData->updateButtonText = (wParam == 2) ? L"Update Found!" : ((wParam == 4) ? L"Error to check" : L"You are up to date!");
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
                        PostMessage(hwnd, WM_APP + 3, found ? 2 : (g_updateCheckFailed.load() ? 4 : 3), 0);
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

        About_Paint_DrawUpdateLine(memDC, pData->updateButtonRect, pData->hFont12, pData->updateButtonText, pData->updateButtonCurrentColor, pData->isHoveringUpdate);

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
void MuteProcessByPid(DWORD pid, bool mute)
{
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    IMMDeviceEnumerator* pEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr) || !pEnumerator) { if (SUCCEEDED(hrCom)) CoUninitialize(); return; }

    IMMDevice* pDevice = NULL;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr) || !pDevice) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return; }

    IAudioSessionManager2* pSessionManager = NULL;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pSessionManager);
    pDevice->Release();
    if (FAILED(hr) || !pSessionManager) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return; }

    IAudioSessionEnumerator* pSessionEnumerator = NULL;
    hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
    pSessionManager->Release();
    if (FAILED(hr) || !pSessionEnumerator) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return; }

    int sessionCount = 0;
    hr = pSessionEnumerator->GetCount(&sessionCount);
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < sessionCount; i++) {
            IAudioSessionControl* pSessionControl = NULL;
            hr = pSessionEnumerator->GetSession(i, &pSessionControl);
            if (FAILED(hr)) continue;

            IAudioSessionControl2* pSessionControl2 = NULL;
            hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
            pSessionControl->Release();
            if (FAILED(hr) || !pSessionControl2) continue;

            DWORD sessionPid = 0;
            hr = pSessionControl2->GetProcessId(&sessionPid);
            if (SUCCEEDED(hr) && sessionPid == pid) {
                ISimpleAudioVolume* pAudioVolume = NULL;
                hr = pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pAudioVolume);
                if (SUCCEEDED(hr) && pAudioVolume) {
                    pAudioVolume->SetMute(mute, NULL);
                    pAudioVolume->Release();
                }
            }
            pSessionControl2->Release();
        }
    }
    pSessionEnumerator->Release();
    pEnumerator->Release();
    if (SUCCEEDED(hrCom)) CoUninitialize();
}
void MuteAllRoblox(bool mute)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snap, &pe))
    {
        do
        {
            if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0 || (g_fishstrapSupport.load() && _wcsicmp(pe.szExeFile, L"eurotrucks2.exe") == 0))
            {
                MuteProcessByPid(pe.th32ProcessID, mute);
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
}
void MuteUnmutedPid()
{
    DWORD pid = g_unmutedPid.load();
    if (pid > 0) {
        MuteProcessByPid(pid, true);
        g_unmutedPid = 0;
    }
}
std::vector<DWORD> FindAllRobloxProcessIds()
{
    std::vector<DWORD> pids;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return pids;

    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snap, &pe))
    {
        do
        {
            if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0 || (g_fishstrapSupport.load() && _wcsicmp(pe.szExeFile, L"eurotrucks2.exe") == 0))
            {
                if (std::find(pids.begin(), pids.end(), pe.th32ProcessID) == pids.end()) {
                    pids.push_back(pe.th32ProcessID);
                }
            }
        } while (Process32Next(snap, &pe));
    }

    CloseHandle(snap);
    return pids;
}
bool IsProcessMutedByPid(DWORD pid)
{
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    bool foundSession = false;
    bool isMuted = false;

    IMMDeviceEnumerator* pEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr) || !pEnumerator) { if (SUCCEEDED(hrCom)) CoUninitialize(); return false; }

    IMMDevice* pDevice = NULL;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr) || !pDevice) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return false; }

    IAudioSessionManager2* pSessionManager = NULL;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pSessionManager);
    pDevice->Release();
    if (FAILED(hr) || !pSessionManager) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return false; }

    IAudioSessionEnumerator* pSessionEnumerator = NULL;
    hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
    pSessionManager->Release();
    if (FAILED(hr) || !pSessionEnumerator) { pEnumerator->Release(); if (SUCCEEDED(hrCom)) CoUninitialize(); return false; }

    int sessionCount = 0;
    hr = pSessionEnumerator->GetCount(&sessionCount);
    if (SUCCEEDED(hr))
    {
        for (int i = 0; i < sessionCount; i++)
        {
            IAudioSessionControl* pSessionControl = NULL;
            hr = pSessionEnumerator->GetSession(i, &pSessionControl);
            if (FAILED(hr) || !pSessionControl) continue;

            IAudioSessionControl2* pSessionControl2 = NULL;
            hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
            pSessionControl->Release();
            if (FAILED(hr) || !pSessionControl2) continue;

            DWORD sessionPid = 0;
            hr = pSessionControl2->GetProcessId(&sessionPid);
            if (SUCCEEDED(hr) && sessionPid == pid)
            {
                ISimpleAudioVolume* pAudioVolume = NULL;
                hr = pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pAudioVolume);
                if (SUCCEEDED(hr) && pAudioVolume)
                {
                    BOOL muted = FALSE;
                    if (SUCCEEDED(pAudioVolume->GetMute(&muted))) {
                        foundSession = true;
                        isMuted = muted != FALSE;
                    }
                    pAudioVolume->Release();
                }
            }

            pSessionControl2->Release();

            if (foundSession) {
                break;
            }
        }
    }

    pSessionEnumerator->Release();
    pEnumerator->Release();
    if (SUCCEEDED(hrCom)) CoUninitialize();
    return foundSession && isMuted;
}
bool AreAllRobloxMuted()
{
    auto pids = FindAllRobloxProcessIds();
    bool foundMutedState = false;
    for (DWORD pid : pids)
    {
        if (!IsProcessMutedByPid(pid)) {
            return false;
        }
        foundMutedState = true;
    }
    return foundMutedState;
}
bool IsUtilsWindowOpacityEnabled()
{
    int overrideValue = g_utilsWindowOpacityOverride.load();
    if (overrideValue != -1) {
        return overrideValue != 0;
    }
    return g_windowOpacity.load();
}
bool IsUtilsSessionWindowOpacityEnabled()
{
    return g_utilsWindowOpacityOverride.load() == 1;
}
bool ShouldMuteRobloxNow()
{
    int overrideValue = g_utilsMuteOverride.load();
    if (overrideValue == 1) {
        return true;
    }
    if (overrideValue == 0) {
        return false;
    }
    return g_autoMute.load() && g_isAfkStarted.load();
}
bool IsUtilsSessionMuteEnabled()
{
    return g_utilsMuteOverride.load() == 1;
}
int GetEffectiveFpsLimit()
{
    int overrideValue = g_utilsFpsLimitOverride.load();
    if (overrideValue != -1) {
        return (std::max)(0, overrideValue);
    }
    return g_fpsLimit;
}
bool IsUtilsSessionFpsLimitEnabled()
{
    return g_utilsFpsLimitOverride.load() > 0;
}
bool ShouldRunFpsCapperNow()
{
    return IsUtilsSessionFpsLimitEnabled() || g_isAfkStarted.load();
}
void RestartFpsCapperForEffectiveLimit()
{
    if (g_isFpsCapperRunning.load()) {
        g_isFpsCapperRunning = false;
        if (g_fpsCapperThread.joinable()) {
            g_fpsCapperThread.join();
        }
    }

    if (GetEffectiveFpsLimit() > 0) {
        g_fpsCapperThread = std::thread(FpsCapperThread);
    }
}
void ResetRobloxSessionEffectsOnExit()
{
    ShowAllRobloxWindows_Multi();
    RefreshRobloxWindowOpacity(true);
    MuteAllRoblox(false);
    g_unmutedPid = 0;

    g_utilsWindowOpacityOverride = -1;
    g_utilsMuteOverride = -1;
    g_utilsFpsLimitOverride = -1;
    RestartFpsCapperForEffectiveLimit();
}
// ==========
void GridSnapRobloxWindows()
{
    auto allWins = FindAllRobloxWindows(true);
    std::vector<HWND> wins;
    wins.reserve(allWins.size());

    auto isGridCandidate = [](HWND w) -> bool {
        if (!IsWindow(w)) return false;
        if (GetWindow(w, GW_OWNER) != NULL) return false;

        LONG exStyle = GetWindowLong(w, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOOLWINDOW) return false;

        wchar_t title[256] = { 0 };
        GetWindowTextW(w, title, ARRAYSIZE(title));
        if (title[0] == L'\0') return false;
        if (_wcsicmp(title, L"Default IME") == 0) return false;

        wchar_t className[128] = { 0 };
        GetClassNameW(w, className, ARRAYSIZE(className));
        if (_wcsicmp(className, L"IME") == 0) return false;
        if (_wcsicmp(className, L"MSCTFIME UI") == 0) return false;

        RECT rc = { 0 };
        if (!GetWindowRect(w, &rc)) return false;
        int wpx = rc.right - rc.left;
        int hpx = rc.bottom - rc.top;
        if (wpx < 220 || hpx < 160) return false;

        return true;
    };

    for (HWND w : allWins) {
        if (!isGridCandidate(w)) {
            continue;
        }

        if (IsIconic(w)) {
            ShowWindow(w, SW_RESTORE);
        } else if (!IsWindowVisible(w)) {
            ShowWindow(w, SW_SHOWNOACTIVATE);
        }

        SetWindowPos(w, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        Sleep(20);

        if (isGridCandidate(w) && IsWindowVisible(w) && !IsIconic(w)) {
            wins.push_back(w);
        }
    }

    if (wins.empty()) return;

    std::sort(wins.begin(), wins.end(), [](HWND a, HWND b) {
        return reinterpret_cast<UINT_PTR>(a) < reinterpret_cast<UINT_PTR>(b);
    });
    wins.erase(std::unique(wins.begin(), wins.end()), wins.end());

    RECT workArea;
    if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0)) {
        GetWindowRect(GetDesktopWindow(), &workArea);
    }

    int edgeMargin = 8;
    int windowGap = 14;
    int screenWidth = workArea.right - workArea.left;
    int screenHeight = workArea.bottom - workArea.top;

    int count = (int)wins.size();
    if (screenWidth <= edgeMargin * 2 || screenHeight <= edgeMargin * 2) return;

    std::sort(wins.begin(), wins.end(), [](HWND a, HWND b) {
        RECT ra{ 0 }, rb{ 0 };
        bool gotA = GetWindowRect(a, &ra) != FALSE;
        bool gotB = GetWindowRect(b, &rb) != FALSE;

        if (gotA && gotB) {
            if (ra.top != rb.top) return ra.top < rb.top;
            if (ra.left != rb.left) return ra.left < rb.left;
        }

        return reinterpret_cast<UINT_PTR>(a) < reinterpret_cast<UINT_PTR>(b);
    });

    const int preferredWinW = 800;
    const int preferredWinH = 600;
    int cols = 1;
    while (cols * cols < count) {
        cols++;
    }
    int rows = (count + cols - 1) / cols;

    int usableWidth = screenWidth - edgeMargin * 2;
    int usableHeight = screenHeight - edgeMargin * 2;
    int availableWidth = usableWidth - (cols - 1) * windowGap;
    int availableHeight = usableHeight - (rows - 1) * windowGap;
    if (availableWidth <= 0 || availableHeight <= 0) {
        return;
    }

    int winWidth = preferredWinW;
    int winHeight = preferredWinH;

    if (cols * winWidth > availableWidth || rows * winHeight > availableHeight) {
        double scaleW = (double)availableWidth / (double)(cols * preferredWinW);
        double scaleH = (double)availableHeight / (double)(rows * preferredWinH);
        double scale = (std::min)(scaleW, scaleH);
        if (scale <= 0.0) {
            scale = 1.0;
        }

        winWidth = (std::max)(220, (int)(preferredWinW * scale));
        winHeight = (std::max)(165, (int)(preferredWinH * scale));
    }

    int startX = workArea.left + edgeMargin;
    int startY = workArea.top + edgeMargin;

    for (size_t i = 0; i < wins.size(); ++i) {
        int col = (int)(i % cols);
        int row = (int)(i / cols);
        int x = startX + col * (winWidth + windowGap);
        int y = startY + row * (winHeight + windowGap);

        SetWindowPos(
            wins[i],
            NULL,
            x,
            y,
            winWidth,
            winHeight,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    for (HWND w : wins) {
        if (!IsWindow(w)) continue;
        BringWindowToTop(w);
        SetForegroundWindow(w);
        Sleep(25);
    }
}
std::vector<UINT_PTR> BuildRobloxWindowSignature(const std::vector<HWND>& wins)
{
    std::vector<UINT_PTR> signature;
    signature.reserve(wins.size());
    for (HWND w : wins)
    {
        signature.push_back(reinterpret_cast<UINT_PTR>(w));
    }
    std::sort(signature.begin(), signature.end());
    return signature;
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
void RestoreUserWindowAfterRobloxAction(HWND previousWindow)
{
    if (g_restoreMethod == 1)
    {
        RestoreForegroundWindow(previousWindow);
    }
    else if (g_restoreMethod == 2)
    {
        Sleep(50);
        RestorePreviousWindowWithAltTab();
    }
}
void ShowAllRobloxWindows_Multi()
{
    auto wins = FindAllRobloxWindows(true);
    for (HWND w : wins)
    {
        if (IsIconic(w))
        {
            ShowWindow(w, SW_RESTORE);
        }
        else if (!IsWindowVisible(w))
        {
            ShowWindow(w, SW_SHOW);
        }
    }
}
void HideAllRobloxWindows()
{
    auto wins = FindAllRobloxWindows(true);
    for (HWND w : wins)
    {
        ShowWindow(w, SW_HIDE);
    }
}
void RefreshRobloxWindowOpacity(bool forceDisable)
{
    auto wins = FindAllRobloxWindows(true);
    bool shouldApplyOpacity = !forceDisable && (IsUtilsWindowOpacityEnabled() || (g_autoOpacity.load() && g_isAfkStarted.load()));
    for (HWND w : wins)
    {
        if (shouldApplyOpacity)
        {
            SetWindowLong(w, GWL_EXSTYLE, GetWindowLong(w, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(w, 0, 180, LWA_ALPHA);
        }
        else
        {
            SetWindowLong(w, GWL_EXSTYLE, GetWindowLong(w, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            RedrawWindow(w, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
        }
    }
}
void ApplyAutoRobloxWindowLayout()
{
    auto wins = FindAllRobloxWindows(true);
    std::lock_guard<std::mutex> guard(g_autoWindowLayoutMutex);

    if (wins.empty())
    {
        g_lastAutoGridWindowSignature.clear();
        return;
    }

    if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load())
    {
        RefreshRobloxWindowOpacity(false);
    }

    if (g_autoGrid.load())
    {
        auto signature = BuildRobloxWindowSignature(wins);
        if (signature != g_lastAutoGridWindowSignature)
        {
            GridSnapRobloxWindows();
            g_lastAutoGridWindowSignature = std::move(signature);
        }
    }
    else
    {
        g_lastAutoGridWindowSignature.clear();
    }
}
bool ExecuteRobloxWindowActionForAll(void(*action)(HWND), int repeatCount)
{
    auto wins = FindAllRobloxWindows(true);
    if (!action || repeatCount <= 0 || wins.empty()) {
        return false;
    }

    HWND previousWindow = GetForegroundWindow();
    bool wasFpsCapperPaused = PauseFpsCapperBeforeAction(FPS_CAPPER_PRE_ACTION_PAUSE_MS, false);

    for (HWND w : wins)
    {
        bool wasVisible = IsWindowVisible(w) != FALSE;
        bool wasMinimized = IsIconic(w);

        if (!wasVisible) {
            ShowWindow(w, SW_SHOW);
        }
        if (wasMinimized) {
            ShowWindow(w, SW_RESTORE);
        }

        SetForegroundWindow(w);
        Sleep(50);

        for (int i = 0; i < repeatCount; ++i) {
            action(w);
        }

        if (!wasVisible) {
            ShowWindow(w, SW_HIDE);
        }
        else if (wasMinimized) {
            ShowWindow(w, SW_MINIMIZE);
        }
    }

    ResumeFpsCapperAfterAction(wasFpsCapperPaused);
    RestoreUserWindowAfterRobloxAction(previousWindow);
    return true;
}
int CloseAllRobloxInstances()
{
    auto wins = FindAllRobloxWindows(true);
    for (HWND w : wins)
    {
        PostMessage(w, WM_CLOSE, 0, 0);
    }

    if (!wins.empty()) {
        Sleep(800);
    }

    int terminatedCount = 0;
    auto pids = FindAllRobloxProcessIds();
    for (DWORD pid : pids)
    {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
        if (!hProcess) {
            continue;
        }

        if (TerminateProcess(hProcess, 0))
        {
            terminatedCount++;
            WaitForSingleObject(hProcess, 1500);
        }

        CloseHandle(hProcess);
    }

    return terminatedCount;
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

// Auto Reset
void AutoReset_Action(HWND target)
{
    keybd_event(VK_ESCAPE, MapVirtualKey(VK_ESCAPE, 0), 0, 0);
    Sleep(ALT_DELAY);
    keybd_event(VK_ESCAPE, MapVirtualKey(VK_ESCAPE, 0), KEYEVENTF_KEYUP, 0);
    Sleep(100);
    
    keybd_event('R', MapVirtualKey('R', 0), 0, 0);
    Sleep(ALT_DELAY);
    keybd_event('R', MapVirtualKey('R', 0), KEYEVENTF_KEYUP, 0);
    Sleep(100);
    
    keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
    Sleep(ALT_DELAY);
    keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), KEYEVENTF_KEYUP, 0);
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

// Fish/Void/Bloxstrap integration
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
    
    const std::wstring candidates[] = {
        dir_path_w + L"\\Settings.json",
        dir_path_w + L"\\AppSettings.json"
    };

    for (const auto& candidate_w : candidates) {
        DWORD attrs = GetFileAttributesW(candidate_w.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            int len = WideCharToMultiByte(CP_UTF8, 0, candidate_w.c_str(), -1, NULL, 0, NULL, NULL);
            std::string settings_path(len, 0);
            WideCharToMultiByte(CP_UTF8, 0, candidate_w.c_str(), -1, &settings_path[0], len, NULL, NULL);
            settings_path.pop_back();
            return settings_path;
        }
    }

    return "";
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
        ShowTrayNotification(L"Fish/Void/Bloxstrap Integration Error", L"Could not find launcher installation path in registry.");
        g_bloxstrapIntegration = false;
        return;
    }

    std::ifstream inFile(settingsPath, std::ios::binary);
    if (!inFile.is_open()) {
        ShowTrayNotification(L"Fish/Void/Bloxstrap Integration Error", L"Could not find/open the launcher settings file.");
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

        std::string selfPath = EscapeJsonStringUtf8(GetSelfExePath());

        std::string integrationJson = "    {\r\n      \"Name\": \"" + integrationName + "\",\r\n      \"Location\": \"" + selfPath + "\",\r\n      \"LaunchArgs\": \"--bloxstrap-integration\",\r\n      \"Delay\": 0,\r\n      \"PreLaunch\": true,\r\n      \"AutoClose\": true\r\n    }";

        size_t integrationsArrayPos = content.find("\"CustomIntegrations\": [");
        if (integrationsArrayPos == std::string::npos) {
            ShowTrayNotification(L"Fish/Void/Bloxstrap Integration Error", L"Could not find 'CustomIntegrations' array in the launcher settings file.");
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
        ShowTrayNotification(L"Fish/Void/Bloxstrap Integration Error", L"Failed to write to the launcher settings file. Check permissions.");
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

    while (g_isFpsCapperRunning)
    {
        int effectiveFpsLimit = GetEffectiveFpsLimit();
        if (!ShouldRunFpsCapperNow() || g_isFpsCapperPaused.load() || effectiveFpsLimit <= 0) {
            Sleep(250);
            continue;
        }
        long long frame_duration_ms = 1000LL / effectiveFpsLimit;

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

bool PauseFpsCapperBeforeAction(DWORD waitMs, bool allowAbort)
{
    bool wasFpsCapperPaused = g_isFpsCapperPaused.load();
    if (GetEffectiveFpsLimit() <= 0 || !g_isFpsCapperRunning.load()) {
        return wasFpsCapperPaused;
    }

    g_isFpsCapperPaused = true;

    DWORD waited = 0;
    while (waited < waitMs)
    {
        if (allowAbort)
        {
            if (g_stopThread.load() || !g_isAfkStarted.load())
            {
                break;
            }
        }

        DWORD waitChunk = (std::min)((DWORD)50, waitMs - waited);
        Sleep(waitChunk);
        waited += waitChunk;
    }

    return wasFpsCapperPaused;
}

void ResumeFpsCapperAfterAction(bool previousPausedState)
{
    g_isFpsCapperPaused = previousPausedState;
}
// ==========

// Forward declarations for MonitorUserActivity
void CreateTrayMenu(bool afk);
void UpdateTrayIcon();

// UserSafe
void MonitorUserActivity()
{
    g_monitorThreadRunning = true;
    g_lastActivityTime = GetTickCount64();

    while (g_monitorThreadRunning)
    {
        int currentMode = g_userSafeMode;

        LASTINPUTINFO lii;
        lii.cbSize = sizeof(LASTINPUTINFO);
        if (GetLastInputInfo(&lii))
        {
            if (lii.dwTime != (DWORD)g_lastActivityTime.load())
            {
                g_lastActivityTime = lii.dwTime;
                if (!g_userActive.load()) {
                    g_userActive = true;
                    g_afkReminderState = 0;
                }
            }
            else if ((GetTickCount() - lii.dwTime) / 1000 >= USER_INACTIVITY_WAIT)
            {
                g_userActive = false;
            }
        }

        if (currentMode == 1)
        {
            bool keyActivity = false;
            for (int i = 1; i < 256; i++)
            {
                if (GetAsyncKeyState(i) & 0x8000)
                {
                    keyActivity = true;
                    break;
                }
            }

            if (keyActivity)
            {
                g_lastActivityTime = GetTickCount64();
                g_userActive = true;
                g_afkReminderState = 0; 
            }
        }

        if (g_afkReminderEnabled.load() && !g_userActive.load() && !g_isAfkStarted.load())
        {
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                if (g_afkReminderState.load() > 0) {
                    g_afkReminderState = 0;
                }
            }
            else
            {
                uint64_t inactiveTime = (GetTickCount64() - g_lastActivityTime.load()) / 1000;

                if (inactiveTime >= AFK_REMINDER_TIME && g_afkReminderState.load() == 0)
                {
                    ShowTrayNotification(L"AntiAFK | Reminder", L"You've been inactive for 18 minutes. Starting AFK prevention soon.");
                    g_afkReminderState = 1;
                }

                if (inactiveTime >= AFK_AUTO_START_TIME && g_afkReminderState.load() < 2)
                {
                    ShowTrayNotification(L"AntiAFK | Auto-Started", L"No activity detected for 19 minutes. Starting AFK prevention.");
                    g_afkStartTime = GetTickCount64();
                    g_isAfkStarted = true;
                    g_manuallyStoppedPids.clear();
                    CreateTrayMenu(true);
                    UpdateTrayIcon();
                    g_afkReminderState = 2;
                    g_cv.notify_all();

                    if (ShouldMuteRobloxNow()) {
                        MuteAllRoblox(true);
                    }

                    if (g_autoHideRoblox.load()) {
                        for (HWND w : wins) {
                            ShowWindow(w, SW_HIDE);
                        }
                    }

                    QueueStatusBarOverlay(L"Auto-started after inactivity", 2200, wins.front());
                    QueueDiscordWebhookEvent(DiscordWebhookEvent::Started, L"Started after inactivity.", false);
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
        g_lastActivityTime = GetTickCount64();
        g_userActive = true;
        g_afkReminderState = 0;
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

std::wstring NormalizeDiscordWebhookUrl(const std::wstring& value)
{
    std::wstring normalized;
    normalized.reserve(value.size());
    for (wchar_t ch : value)
    {
        if (!iswspace(ch))
        {
            normalized.push_back(ch);
        }
    }
    if (normalized.size() > DISCORD_WEBHOOK_MAX_LENGTH)
    {
        normalized.resize(DISCORD_WEBHOOK_MAX_LENGTH);
    }
    return normalized;
}

std::wstring GetDiscordWebhookUrlCopy()
{
    std::lock_guard<std::mutex> lock(g_discordWebhookMutex);
    return g_discordWebhookUrl;
}

void SetDiscordWebhookUrl(const std::wstring& value)
{
    std::lock_guard<std::mutex> lock(g_discordWebhookMutex);
    g_discordWebhookUrl = NormalizeDiscordWebhookUrl(value);
}

bool IsDiscordWebhookUrl(const std::wstring& url)
{
    std::wstring normalized = NormalizeDiscordWebhookUrl(url);
    if (normalized.empty()) return false;

    URL_COMPONENTS components = { 0 };
    wchar_t host[256] = { 0 };
    wchar_t path[2048] = { 0 };
    components.dwStructSize = sizeof(components);
    components.lpszHostName = host;
    components.dwHostNameLength = _countof(host);
    components.lpszUrlPath = path;
    components.dwUrlPathLength = _countof(path);

    if (!InternetCrackUrlW(normalized.c_str(), 0, 0, &components))
    {
        return false;
    }

    std::wstring hostName(components.lpszHostName, components.dwHostNameLength);
    std::wstring urlPath(components.lpszUrlPath, components.dwUrlPathLength);
    std::transform(hostName.begin(), hostName.end(), hostName.begin(), towlower);

    bool isHttps = components.nScheme == INTERNET_SCHEME_HTTPS;
    bool isDiscordHost = hostName == L"discord.com" || hostName == L"discordapp.com" ||
        hostName == L"canary.discord.com" || hostName == L"ptb.discord.com";

    return isHttps && isDiscordHost && urlPath.find(L"/api/webhooks/") == 0;
}

std::wstring Utf8ToWide(const std::string& input)
{
    if (input.empty()) return L"";

    int needed = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), NULL, 0);
    if (needed <= 0) return L"";

    std::wstring output(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), &output[0], needed);
    return output;
}

std::string WideToUtf8(const std::wstring& input)
{
    if (input.empty()) return "";

    int needed = WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), NULL, 0, NULL, NULL);
    if (needed <= 0) return "";

    std::string output(needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), &output[0], needed, NULL, NULL);
    return output;
}

std::string EscapeJsonStringUtf8(const std::string& input)
{
    std::string escaped;
    escaped.reserve(input.size() + 16);

    for (unsigned char ch : input)
    {
        switch (ch)
        {
        case '\"': escaped += "\\\""; break;
        case '\\': escaped += "\\\\"; break;
        case '\b': escaped += "\\b"; break;
        case '\f': escaped += "\\f"; break;
        case '\n': escaped += "\\n"; break;
        case '\r': escaped += "\\r"; break;
        case '\t': escaped += "\\t"; break;
        default:
            if (ch < 0x20)
            {
                char buffer[7];
                sprintf_s(buffer, "\\u%04X", ch);
                escaped += buffer;
            }
            else
            {
                escaped.push_back((char)ch);
            }
            break;
        }
    }

    return escaped;
}

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static int NormalizeSelectedTimeValue(int value)
{
    return ClampInt(value, 1, 24 * 60 * 60);
}

static int NormalizeSelectedAction(int value)
{
    return ClampInt(value, 0, 2);
}

static int NormalizeUserSafeMode(int value)
{
    return ClampInt(value, 0, 2);
}

static int NormalizeRestoreMethod(int value)
{
    return ClampInt(value, 0, 2);
}

static int NormalizeFpsLimitValue(int value)
{
    return ClampInt(value, 0, 60);
}

static int NormalizeMultiInstanceIntervalValue(int value)
{
    switch (value)
    {
    case 0:
    case 1000:
    case 3000:
    case 5000:
    case 10000:
        return value;
    default:
        return 0;
    }
}

std::wstring GetWin32ErrorText(DWORD error)
{
    if (error == 0) return L"Unknown error.";

    LPWSTR buffer = NULL;
    DWORD length = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&buffer,
        0,
        NULL);

    std::wstring message = (length > 0 && buffer) ? buffer : L"Unknown error.";
    if (buffer)
    {
        LocalFree(buffer);
    }

    while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n'))
    {
        message.pop_back();
    }
    return message;
}

std::wstring GetDiscordEventLabel(DiscordWebhookEvent eventType)
{
    switch (eventType)
    {
    case DiscordWebhookEvent::Test: return L"Test";
    case DiscordWebhookEvent::Started: return L"Started";
    case DiscordWebhookEvent::Stopped: return L"Stopped";
    case DiscordWebhookEvent::Action: return L"AFK Action";
    case DiscordWebhookEvent::AutoReconnect: return L"Auto Reconnect";
    case DiscordWebhookEvent::AutoReset: return L"Auto Reset";
    case DiscordWebhookEvent::Error: return L"Error";
    }
    return L"Unknown";
}

DWORD GetDiscordEventColor(DiscordWebhookEvent eventType)
{
    switch (eventType)
    {
    case DiscordWebhookEvent::Test: return 0x007ACC;
    case DiscordWebhookEvent::Started: return 0x007ACC;
    case DiscordWebhookEvent::Stopped: return 0xC81E1E;
    case DiscordWebhookEvent::Action: return 0x007ACC;
    case DiscordWebhookEvent::AutoReconnect: return 0x007ACC;
    case DiscordWebhookEvent::AutoReset: return 0x007ACC;
    case DiscordWebhookEvent::Error: return 0xC81E1E;
    }
    return 0x007ACC;
}

std::wstring GetDiscordUtcTimestamp()
{
    SYSTEMTIME utcTime = { 0 };
    GetSystemTime(&utcTime);

    wchar_t buffer[64];
    swprintf_s(
        buffer,
        L"%04d-%02d-%02dT%02d:%02d:%02d.000Z",
        utcTime.wYear,
        utcTime.wMonth,
        utcTime.wDay,
        utcTime.wHour,
        utcTime.wMinute,
        utcTime.wSecond);
    return buffer;
}

std::wstring GetDiscordDisplayTimeLabel()
{
    SYSTEMTIME localTime = { 0 };
    GetLocalTime(&localTime);

    wchar_t buffer[64];
    swprintf_s(buffer, L"today at %02d:%02d", localTime.wHour, localTime.wMinute);
    return buffer;
}

std::wstring GetDiscordActionCompactLabel()
{
    switch (g_selectedAction)
    {
    case 0: return L"Jump";
    case 1: return L"W/S";
    case 2: return L"Zoom";
    default: return L"Action";
    }
}

std::wstring GetDiscordIntervalCompactLabel()
{
    wchar_t buffer[32];
    if (g_selectedTime % 60 == 0)
    {
        swprintf_s(buffer, L"%d min", g_selectedTime / 60);
    }
    else
    {
        swprintf_s(buffer, L"%d sec", g_selectedTime);
    }
    return buffer;
}

std::wstring BuildDiscordStartedDetailsLine()
{
    std::wstring line = GetDiscordActionCompactLabel();
    line += L" \u2022 ";
    line += GetDiscordIntervalCompactLabel();

    if (g_autoReconnect.load())
    {
        line += L" \u2022 **Reconnect**";
    }
    if (g_autoReset.load())
    {
        line += L" \u2022 **Reset**";
    }
    if (g_doNotSleep.load())
    {
        line += L" \u2022 **Do Not Sleep**";
    }
    if (g_fpsLimit > 0)
    {
        line += L" \u2022 **FPS Capper**";
    }

    return line;
}

std::wstring BuildDiscordStartedInstancesLine()
{
    auto wins = FindAllRobloxWindows(false);
    std::vector<DWORD> uniquePids;
    uniquePids.reserve(wins.size());

    for (HWND w : wins)
    {
        if (!IsWindowVisible(w) || IsIconic(w))
        {
            continue;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid != 0 && std::find(uniquePids.begin(), uniquePids.end(), pid) == uniquePids.end())
        {
            uniquePids.push_back(pid);
        }
    }

    size_t count = uniquePids.size();

    wchar_t buffer[96];
    swprintf_s(buffer, L"Started \u2022 Found %zu %s", count, count == 1 ? L"Window" : L"Windows");
    return buffer;
}

std::wstring BuildDiscordStartedCompactInstancesLabel()
{
    std::wstring fullLabel = BuildDiscordStartedInstancesLine();
    const std::wstring prefix = L"Started \u2022 Found ";
    if (fullLabel.rfind(prefix, 0) == 0)
    {
        std::wstring suffix = fullLabel.substr(prefix.size());
        return suffix + L" found";
    }
    return fullLabel;
}

std::wstring BuildDiscordAsciiDescription(DiscordWebhookEvent eventType, const std::wstring& summary)
{
    std::wstring description;
    description.reserve(256);

    auto appendLine = [&](const std::wstring& line)
    {
        if (!description.empty())
        {
            description += L"\n";
        }
        description += line;
    };

    switch (eventType)
    {
    case DiscordWebhookEvent::Started:
        appendLine(BuildDiscordStartedInstancesLine());
        appendLine(BuildDiscordStartedDetailsLine());
        break;
    case DiscordWebhookEvent::Stopped:
        if (summary.find(L"manual") != std::wstring::npos || summary.find(L"Manual") != std::wstring::npos)
        {
            appendLine(L"Stopped \u2022 Manual stop");
        }
        else if (summary.find(L"Roblox not found") != std::wstring::npos)
        {
            appendLine(L"Stopped \u2022 Roblox not found");
        }
        else if (!summary.empty())
        {
            appendLine(std::wstring(L"Stopped \u2022 ") + summary);
        }
        else
        {
            appendLine(L"Stopped");
        }
        break;
    case DiscordWebhookEvent::Error:
        if (summary.find(L"Roblox not found") != std::wstring::npos)
        {
            appendLine(L"Error \u2022 Roblox not found");
        }
        else if (!summary.empty())
        {
            appendLine(std::wstring(L"Error \u2022 ") + summary);
        }
        else
        {
            appendLine(L"Error");
        }
        break;
    case DiscordWebhookEvent::AutoReconnect:
        appendLine(L"Reconnected \u2022 Roblox");
        break;
    case DiscordWebhookEvent::Action:
        appendLine(std::wstring(L"Action \u2022 ") + GetDiscordActionCompactLabel() + L" \u2022 Next in " + GetDiscordIntervalCompactLabel());
        break;
    case DiscordWebhookEvent::Test:
        appendLine(L"Test \u2022 Webhook connected");
        break;
    case DiscordWebhookEvent::AutoReset:
        appendLine(L"Reset");
        if (!summary.empty())
        {
            appendLine(summary);
        }
        break;
    default:
        appendLine(GetDiscordEventLabel(eventType));
        if (!summary.empty())
        {
            appendLine(summary);
        }
        break;
    }

    return description;
}

std::wstring GetDiscordCompactVersionLabel()
{
    std::wstring version = g_Version ? g_Version : L"v";
    if (version.rfind(L"v.", 0) == 0)
    {
        version.erase(1, 1);
    }
    if (version.size() > 2 && version.rfind(L".0") == version.size() - 2)
    {
        version.resize(version.size() - 2);
    }
    return version;
}

std::wstring GetDiscordInviteUrl()
{
    return L"https://agzes.github.io/go/to/discord";
}

std::wstring GetGithubRepoUrl()
{
    return L"https://github.com/Agzes/AntiAFK-RBX";
}

std::wstring GetDiscordAuthorIconUrl()
{
    return L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/Resources/tray-icon-off.png";
}

std::wstring BuildDiscordEmbedTitle(DiscordWebhookEvent eventType)
{
    switch (eventType)
    {
    case DiscordWebhookEvent::Started: return L"AntiAFK Started";
    case DiscordWebhookEvent::Stopped: return L"AntiAFK Stopped";
    case DiscordWebhookEvent::Action: return L"AntiAFK Action";
    case DiscordWebhookEvent::AutoReconnect: return L"AntiAFK Reconnect";
    case DiscordWebhookEvent::AutoReset: return L"AntiAFK Reset";
    case DiscordWebhookEvent::Error: return L"AntiAFK Error";
    case DiscordWebhookEvent::Test: return L"AntiAFK Test";
    }
    return L"AntiAFK Update";
}

std::wstring BuildDiscordEmbedDescription(DiscordWebhookEvent eventType, const std::wstring& summary)
{
    switch (eventType)
    {
    case DiscordWebhookEvent::Started:
        return BuildDiscordStartedCompactInstancesLabel() + L" \u2022 " + BuildDiscordStartedDetailsLine();
    case DiscordWebhookEvent::Stopped:
        if (summary.find(L"manual") != std::wstring::npos || summary.find(L"Manual") != std::wstring::npos)
        {
            return L"Manual stop";
        }
        if (summary.find(L"Roblox not found") != std::wstring::npos)
        {
            return L"Roblox not found";
        }
        return summary.empty() ? L"Stopped" : summary;
    case DiscordWebhookEvent::Error:
        if (summary.find(L"Roblox not found") != std::wstring::npos)
        {
            return L"Roblox not found";
        }
        return summary.empty() ? L"Unknown error" : summary;
    case DiscordWebhookEvent::AutoReconnect:
        return summary.empty() ? L"Reconnect triggered" : summary;
    case DiscordWebhookEvent::Action:
        return GetDiscordActionCompactLabel() + L" \u2022 Next in " + GetDiscordIntervalCompactLabel();
    case DiscordWebhookEvent::Test:
        return L"Webhook connected";
    case DiscordWebhookEvent::AutoReset:
        return summary.empty() ? L"Reset triggered" : summary;
    default:
        return summary.empty() ? GetDiscordEventLabel(eventType) : summary;
    }
}

bool ShouldMentionDiscordEvent(DiscordWebhookEvent eventType)
{
    if (!g_discordMentionOnErrors.load())
    {
        return false;
    }

    switch (eventType)
    {
    case DiscordWebhookEvent::Error:
    case DiscordWebhookEvent::AutoReconnect:
        return true;
    default:
        return false;
    }
}

std::wstring BuildDiscordPlainContentLine(DiscordWebhookEvent eventType, const std::wstring& summary)
{
    if (eventType == DiscordWebhookEvent::Started)
    {
        return std::wstring(L"AntiAFK-RBX ") + GetDiscordCompactVersionLabel() + L" \u2022 Started \u2022 " + BuildDiscordStartedCompactInstancesLabel();
    }

    std::wstring content = BuildDiscordAsciiDescription(eventType, summary);
    for (wchar_t& ch : content)
    {
        if (ch == L'\n' || ch == L'\r')
        {
            ch = L' ';
        }
    }

    std::wstring compact;
    compact.reserve(content.size());
    bool previousWasSpace = false;
    for (wchar_t ch : content)
    {
        bool isSpace = iswspace(ch) != 0;
        if (isSpace)
        {
            if (!previousWasSpace)
            {
                compact.push_back(L' ');
            }
        }
        else
        {
            compact.push_back(ch);
        }
        previousWasSpace = isSpace;
    }

    while (!compact.empty() && compact.front() == L' ')
    {
        compact.erase(compact.begin());
    }
    while (!compact.empty() && compact.back() == L' ')
    {
        compact.pop_back();
    }

    compact = std::wstring(L"AntiAFK-RBX ") + GetDiscordCompactVersionLabel() + L" \u2022 " + compact;
    return compact;
}

std::string BuildDiscordWebhookPayloadJson(DiscordWebhookEvent eventType, const std::wstring& summary)
{
    std::wstring mentionText = ShouldMentionDiscordEvent(eventType) ? L"@everyone" : L"";
    std::string mentionUtf8 = EscapeJsonStringUtf8(WideToUtf8(mentionText));
    const char* allowedMentionsJson = mentionText.empty()
        ? "{\"parse\":[]}"
        : "{\"parse\":[\"everyone\"]}";

    if (g_discordDisableEmbed.load())
    {
        std::wstring content = BuildDiscordPlainContentLine(eventType, summary);
        if (!mentionText.empty())
        {
            content = mentionText + L" " + content;
        }
        std::string contentUtf8 = EscapeJsonStringUtf8(WideToUtf8(content));

        std::ostringstream json;
        json
            << "{\"username\":\"AntiAFK-RBX\",\"allowed_mentions\":" << allowedMentionsJson << ",\"content\":\""
            << contentUtf8
            << "\"}";
        return json.str();
    }

    std::wstring title = BuildDiscordEmbedTitle(eventType);
    std::wstring description = BuildDiscordEmbedDescription(eventType, summary);
    std::wstring authorName = L"AntiAFK-RBX " + GetDiscordCompactVersionLabel();
    std::wstring authorUrl = GetGithubRepoUrl();
    std::wstring authorIconUrl = GetDiscordAuthorIconUrl();

    std::string titleUtf8 = EscapeJsonStringUtf8(WideToUtf8(title));
    std::string descriptionUtf8 = EscapeJsonStringUtf8(WideToUtf8(description));
    std::string authorNameUtf8 = EscapeJsonStringUtf8(WideToUtf8(authorName));
    std::string authorUrlUtf8 = EscapeJsonStringUtf8(WideToUtf8(authorUrl));
    std::string authorIconUrlUtf8 = EscapeJsonStringUtf8(WideToUtf8(authorIconUrl));

    std::ostringstream json;
    json
        << "{\"allowed_mentions\":" << allowedMentionsJson << ",\"content\":";
    if (!mentionText.empty())
    {
        json << "\"" << mentionUtf8 << "\"";
    }
    else
    {
        json << "null";
    }
    json
        << ",\"embeds\":[{"
        << "\"title\":\"" << titleUtf8 << "\","
        << "\"description\":\"" << descriptionUtf8 << "\","
        << "\"author\":{\"name\":\"" << authorNameUtf8 << "\",\"url\":\"" << authorUrlUtf8 << "\",\"icon_url\":\"" << authorIconUrlUtf8 << "\"},"
        << "\"color\":" << GetDiscordEventColor(eventType)
        << "}],\"attachments\":[]}";
    return json.str();
}

uint64_t FinalizeAfkSession()
{
    uint64_t startTime = g_afkStartTime.exchange(0);
    if (startTime == 0)
    {
        return 0;
    }

    uint64_t sessionSeconds = (GetTickCount64() - startTime) / 1000;
    g_totalAfkTimeSeconds += sessionSeconds;
    g_afkSessionsCompleted++;

    uint64_t longestSession = g_longestAfkSessionSeconds.load();
    while (sessionSeconds > longestSession && !g_longestAfkSessionSeconds.compare_exchange_weak(longestSession, sessionSeconds))
    {
    }

    return sessionSeconds;
}

std::wstring FormatDurationShort(uint64_t totalSeconds)
{
    uint64_t hours = totalSeconds / 3600;
    uint64_t minutes = (totalSeconds % 3600) / 60;

    wchar_t buffer[64];
    swprintf_s(buffer, L"%llu h %llu min", hours, minutes);
    return buffer;
}

bool ShouldSendDiscordWebhookEvent(DiscordWebhookEvent eventType, bool bypassEnabledCheck = false)
{
    if (eventType == DiscordWebhookEvent::Test)
    {
        return true;
    }

    if (!bypassEnabledCheck && !g_discordWebhookEnabled.load())
    {
        return false;
    }

    switch (eventType)
    {
    case DiscordWebhookEvent::Started: return g_discordNotifyStart.load();
    case DiscordWebhookEvent::Stopped: return g_discordNotifyStart.load();
    case DiscordWebhookEvent::Action: return g_discordNotifyAction.load();
    case DiscordWebhookEvent::AutoReconnect: return g_discordNotifyReconnect.load();
    case DiscordWebhookEvent::AutoReset: return false;
    case DiscordWebhookEvent::Error: return g_discordNotifyErrors.load();
    case DiscordWebhookEvent::Test: return true;
    }
    return false;
}

bool ReadUnicodeTextFromClipboard(HWND owner, std::wstring& outText)
{
    outText.clear();
    if (!OpenClipboard(owner))
    {
        return false;
    }

    HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
    if (!clipboardData)
    {
        CloseClipboard();
        return false;
    }

    const wchar_t* rawText = (const wchar_t*)GlobalLock(clipboardData);
    if (!rawText)
    {
        CloseClipboard();
        return false;
    }

    outText = rawText;
    GlobalUnlock(clipboardData);
    CloseClipboard();
    return true;
}

bool TryParseOnOffValue(const std::wstring& value, bool& outValue)
{
    if (value == L"on")
    {
        outValue = true;
        return true;
    }
    if (value == L"off")
    {
        outValue = false;
        return true;
    }
    return false;
}

bool ReadOptionalOnOffArgument(LPWSTR* argv, int argc, int& index, bool defaultValue, bool& outValue)
{
    outValue = defaultValue;
    if (index + 1 >= argc)
    {
        return false;
    }

    bool parsedValue = false;
    if (!TryParseOnOffValue(argv[index + 1], parsedValue))
    {
        return false;
    }

    outValue = parsedValue;
    ++index;
    return true;
}

void ResetStatisticsCounters()
{
    g_totalAfkTimeSeconds = 0;
    g_afkActionsPerformed = 0;
    g_autoReconnectsPerformed = 0;
    g_longestAfkSessionSeconds = 0;
    g_discordWebhooksSent = 0;
    g_programLaunches = 0;
    g_afkSessionsCompleted = 0;
    SaveSettings();
}

bool SendDiscordWebhookRequest(const std::wstring& webhookUrl, DiscordWebhookEvent eventType, const std::wstring& summary, std::wstring* errorOut = nullptr, DWORD* statusCodeOut = nullptr)
{
    std::wstring normalizedUrl = NormalizeDiscordWebhookUrl(webhookUrl);
    if (!IsDiscordWebhookUrl(normalizedUrl))
    {
        if (errorOut) *errorOut = L"Webhook URL is empty or invalid.";
        return false;
    }

    URL_COMPONENTS components = { 0 };
    wchar_t host[256] = { 0 };
    wchar_t path[2048] = { 0 };
    wchar_t extraInfo[2048] = { 0 };
    components.dwStructSize = sizeof(components);
    components.lpszHostName = host;
    components.dwHostNameLength = _countof(host);
    components.lpszUrlPath = path;
    components.dwUrlPathLength = _countof(path);
    components.lpszExtraInfo = extraInfo;
    components.dwExtraInfoLength = _countof(extraInfo);

    if (!InternetCrackUrlW(normalizedUrl.c_str(), 0, 0, &components))
    {
        if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());
        return false;
    }

    std::wstring requestPath(path, components.dwUrlPathLength);
    requestPath.append(extraInfo, components.dwExtraInfoLength);

    HINTERNET hInternet = InternetOpenW(L"AntiAFK-RBX/3.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
    {
        if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());    
        return false;
    }

    bool success = false;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    do
    {
        hConnect = InternetConnectW(
            hInternet,
            host,
            components.nPort,
            NULL,
            NULL,
            INTERNET_SERVICE_HTTP,
            0,
            0);
        if (!hConnect)
        {
            if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());
            break;
        }

        const wchar_t* acceptTypes[] = { L"*/*", NULL };
        hRequest = HttpOpenRequestW(
            hConnect,
            L"POST",
            requestPath.c_str(),
            NULL,
            NULL,
            acceptTypes,
            INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI,
            0);
        if (!hRequest)
        {
            if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());
            break;
        }

        std::string jsonBody = BuildDiscordWebhookPayloadJson(eventType, summary);

        const wchar_t* headers = L"Content-Type: application/json\r\n";
        if (!HttpSendRequestW(hRequest, headers, -1, (LPVOID)jsonBody.data(), (DWORD)jsonBody.size()))
        {
            if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());
            break;
        }

        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        if (!HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusSize, NULL))
        {
            if (errorOut) *errorOut = GetWin32ErrorText(GetLastError());
            break;
        }

        if (statusCodeOut) *statusCodeOut = statusCode;

        if (statusCode >= 200 && statusCode < 300)
        {
            success = true;
            g_discordWebhooksSent++;
            break;
        }

        std::string responseBody;
        char buffer[512];
        DWORD bytesRead = 0;
        while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            responseBody.append(buffer, bytesRead);
            bytesRead = 0;
        }

        if (errorOut)
        {
            std::wstringstream errorStream;
            errorStream << L"Discord returned HTTP " << statusCode;
            if (!responseBody.empty())
            {
                std::wstring bodyText = Utf8ToWide(responseBody);
                if (!bodyText.empty())
                {
                    errorStream << L": " << bodyText;
                }
            }
            *errorOut = errorStream.str();
        }
    } while (false);

    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return success;
}

void QueueDiscordWebhookEvent(DiscordWebhookEvent eventType, const std::wstring& summary, bool bypassEnabledCheck)
{
    if (!ShouldSendDiscordWebhookEvent(eventType, bypassEnabledCheck))
    {
        return;
    }

    std::wstring webhookUrl = GetDiscordWebhookUrlCopy();
    if (!IsDiscordWebhookUrl(webhookUrl))
    {
        return;
    }

    std::thread([webhookUrl, eventType, summary]() {
        SendDiscordWebhookRequest(webhookUrl, eventType, summary, nullptr, nullptr);
        }).detach();
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
    DWORD autoReset = g_autoReset.load();
    DWORD autoHideRoblox = g_autoHideRoblox.load();
    DWORD autoOpacity = g_autoOpacity.load();
    DWORD autoGrid = g_autoGrid.load();
    DWORD restoreMethod = g_restoreMethod;
    DWORD tutorialShown = g_tutorialShown.load();
    DWORD bloxstrapIntegration = g_bloxstrapIntegration.load();
    DWORD statusBarEnabled = g_statusBarEnabled.load();
    DWORD unlockFpsOnFocus = g_unlockFpsOnFocus.load();
    uint64_t afkActions = g_afkActionsPerformed.load();
    uint64_t autoReconnects = g_autoReconnectsPerformed.load();
    DWORD useLegacyUi = g_useLegacyUi.load();
    DWORD fpsLimit = g_fpsLimit;
    DWORD windowOpacity = g_windowOpacity.load();
    DWORD afkReminder = g_afkReminderEnabled.load();
    DWORD doNotSleep = g_doNotSleep.load();
    DWORD autoMute = g_autoMute.load();
    DWORD unmuteOnFocus = g_unmuteOnFocus.load();
    DWORD discordWebhookEnabled = g_discordWebhookEnabled.load();
    DWORD discordNotifyStart = g_discordNotifyStart.load();
    DWORD discordNotifyStop = g_discordNotifyStart.load();
    DWORD discordNotifyAction = g_discordNotifyAction.load();
    DWORD discordNotifyReconnect = g_discordNotifyReconnect.load();
    DWORD discordNotifyReset = g_discordNotifyReset.load();
    DWORD discordNotifyErrors = g_discordNotifyErrors.load();
    DWORD discordDisableEmbed = g_discordDisableEmbed.load();
    DWORD discordMentionOnErrors = g_discordMentionOnErrors.load();
    uint64_t totalAfkTime = g_totalAfkTimeSeconds.load();
    uint64_t longestAfkSession = g_longestAfkSessionSeconds.load();
    uint64_t discordWebhooksSent = g_discordWebhooksSent.load();
    uint64_t programLaunches = g_programLaunches.load();
    uint64_t afkSessionsCompleted = g_afkSessionsCompleted.load();
    std::wstring discordWebhookUrl = GetDiscordWebhookUrlCopy();


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
        RegSetValueEx(hKey, L"AutoReset", 0, REG_DWORD, (const BYTE*)&autoReset, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoHideRoblox", 0, REG_DWORD, (const BYTE*)&autoHideRoblox, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoOpacity", 0, REG_DWORD, (const BYTE*)&autoOpacity, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoGrid", 0, REG_DWORD, (const BYTE*)&autoGrid, sizeof(DWORD));
        RegSetValueEx(hKey, L"RestoreMethod", 0, REG_DWORD, (const BYTE*)&restoreMethod, sizeof(DWORD));
        RegSetValueEx(hKey, L"TutorialShown", 0, REG_DWORD, (const BYTE*)&tutorialShown, sizeof(DWORD));
        RegSetValueEx(hKey, L"TotalAfkTime", 0, REG_QWORD, (const BYTE*)&totalAfkTime, sizeof(uint64_t));
        RegSetValueEx(hKey, L"AfkActions", 0, REG_QWORD, (const BYTE*)&afkActions, sizeof(uint64_t));
        RegSetValueEx(hKey, L"AutoReconnects", 0, REG_QWORD, (const BYTE*)&autoReconnects, sizeof(uint64_t));
        RegSetValueEx(hKey, L"LongestAfkSession", 0, REG_QWORD, (const BYTE*)&longestAfkSession, sizeof(uint64_t));
        RegSetValueEx(hKey, L"DiscordWebhooksSent", 0, REG_QWORD, (const BYTE*)&discordWebhooksSent, sizeof(uint64_t));
        RegSetValueEx(hKey, L"ProgramLaunches", 0, REG_QWORD, (const BYTE*)&programLaunches, sizeof(uint64_t));
        RegSetValueEx(hKey, L"AfkSessionsCompleted", 0, REG_QWORD, (const BYTE*)&afkSessionsCompleted, sizeof(uint64_t));

        RegSetValueEx(hKey, L"BloxstrapIntegration", 0, REG_DWORD, (const BYTE*)&bloxstrapIntegration, sizeof(DWORD));
        RegSetValueEx(hKey, L"UseLegacyUI", 0, REG_DWORD, (const BYTE*)&useLegacyUi, sizeof(DWORD));
        RegSetValueEx(hKey, L"StatusBarEnabled", 0, REG_DWORD, (const BYTE*)&statusBarEnabled, sizeof(DWORD));
        RegSetValueEx(hKey, L"FpsLimit", 0, REG_DWORD, (const BYTE*)&fpsLimit, sizeof(DWORD));
        RegSetValueEx(hKey, L"UnlockFpsOnFocus", 0, REG_DWORD, (const BYTE*)&unlockFpsOnFocus, sizeof(DWORD));
        RegSetValueEx(hKey, L"MultiInstanceInterval", 0, REG_DWORD, (const BYTE*)&g_multiInstanceInterval, sizeof(DWORD));
        RegSetValueEx(hKey, L"WindowOpacity", 0, REG_DWORD, (const BYTE*)&windowOpacity, sizeof(DWORD));
        RegSetValueEx(hKey, L"AfkReminder", 0, REG_DWORD, (const BYTE*)&afkReminder, sizeof(DWORD));
        RegSetValueEx(hKey, L"DoNotSleep", 0, REG_DWORD, (const BYTE*)&doNotSleep, sizeof(DWORD));
        RegSetValueEx(hKey, L"AutoMute", 0, REG_DWORD, (const BYTE*)&autoMute, sizeof(DWORD));
        RegSetValueEx(hKey, L"UnmuteOnFocus", 0, REG_DWORD, (const BYTE*)&unmuteOnFocus, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordWebhookEnabled", 0, REG_DWORD, (const BYTE*)&discordWebhookEnabled, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyStart", 0, REG_DWORD, (const BYTE*)&discordNotifyStart, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyStop", 0, REG_DWORD, (const BYTE*)&discordNotifyStop, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyAction", 0, REG_DWORD, (const BYTE*)&discordNotifyAction, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyReconnect", 0, REG_DWORD, (const BYTE*)&discordNotifyReconnect, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyReset", 0, REG_DWORD, (const BYTE*)&discordNotifyReset, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordNotifyErrors", 0, REG_DWORD, (const BYTE*)&discordNotifyErrors, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordDisableEmbed", 0, REG_DWORD, (const BYTE*)&discordDisableEmbed, sizeof(DWORD));
        RegSetValueEx(hKey, L"DiscordMentionOnErrors", 0, REG_DWORD, (const BYTE*)&discordMentionOnErrors, sizeof(DWORD));
        RegSetValueExW(hKey, L"DiscordWebhookUrl", 0, REG_SZ, (const BYTE*)discordWebhookUrl.c_str(), (DWORD)((discordWebhookUrl.size() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}
void LoadSettings()
{
    HKEY hKey;
   DWORD multiSupport = 0, fishstrapSupport = 0, selectedTime = 540, selectedAction = 0, autoStartAfk = 0;
    DWORD autoUpdate = 1, userSafeMode = 0, autoReconnect = 0, autoReset = 0, autoHideRoblox = 0, autoOpacity = 0, autoGrid = 0, autoLimitRam = 0, restoreMethod = 1;
    DWORD tutorialShown = 0, useLegacyUi = 0, bloxstrapIntegration = 0, statusBarEnabled = 1, fpsLimit = 0, unlockFpsOnFocus = 0;
    DWORD multiInstanceInterval = 0, windowOpacity = 0, afkReminder = 0, doNotSleep = 0, autoMute = 0, unmuteOnFocus = 0;
    DWORD discordWebhookEnabled = 0, discordNotifyStart = 1, discordNotifyStop = 1, discordNotifyAction = 0, discordNotifyReconnect = 1, discordNotifyReset = 0, discordNotifyErrors = 1, discordDisableEmbed = 0, discordMentionOnErrors = 0;
    uint64_t totalAfkTime = 0, afkActions = 0, autoReconnects = 0, longestAfkSession = 0, discordWebhooksSent = 0, programLaunches = 0, afkSessionsCompleted = 0;
    DWORD dataSize = sizeof(DWORD);
    DWORD dataSize64 = sizeof(uint64_t);
    std::wstring discordWebhookUrl;

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
        RegQueryValueEx(hKey, L"AutoReset", NULL, NULL, (LPBYTE)&autoReset, &dataSize);
        RegQueryValueEx(hKey, L"AutoHideRoblox", NULL, NULL, (LPBYTE)&autoHideRoblox, &dataSize);
        RegQueryValueEx(hKey, L"AutoOpacity", NULL, NULL, (LPBYTE)&autoOpacity, &dataSize);
        RegQueryValueEx(hKey, L"AutoGrid", NULL, NULL, (LPBYTE)&autoGrid, &dataSize);
        RegQueryValueEx(hKey, L"RestoreMethod", NULL, NULL, (LPBYTE)&restoreMethod, &dataSize);
        RegQueryValueEx(hKey, L"TutorialShown", NULL, NULL, (LPBYTE)&tutorialShown, &dataSize);
        RegQueryValueEx(hKey, L"TotalAfkTime", NULL, NULL, (LPBYTE)&totalAfkTime, &dataSize64);
        RegQueryValueEx(hKey, L"AfkActions", NULL, NULL, (LPBYTE)&afkActions, &dataSize64);
        RegQueryValueEx(hKey, L"AutoReconnects", NULL, NULL, (LPBYTE)&autoReconnects, &dataSize64);
        RegQueryValueEx(hKey, L"LongestAfkSession", NULL, NULL, (LPBYTE)&longestAfkSession, &dataSize64);
        RegQueryValueEx(hKey, L"DiscordWebhooksSent", NULL, NULL, (LPBYTE)&discordWebhooksSent, &dataSize64);
        RegQueryValueEx(hKey, L"ProgramLaunches", NULL, NULL, (LPBYTE)&programLaunches, &dataSize64);
        RegQueryValueEx(hKey, L"AfkSessionsCompleted", NULL, NULL, (LPBYTE)&afkSessionsCompleted, &dataSize64);
        RegQueryValueEx(hKey, L"UseLegacyUI", NULL, NULL, (LPBYTE)&useLegacyUi, &dataSize);
        RegQueryValueEx(hKey, L"BloxstrapIntegration", NULL, NULL, (LPBYTE)&bloxstrapIntegration, &dataSize);
        RegQueryValueEx(hKey, L"StatusBarEnabled", NULL, NULL, (LPBYTE)&statusBarEnabled, &dataSize);
        RegQueryValueEx(hKey, L"FpsLimit", NULL, NULL, (LPBYTE)&fpsLimit, &dataSize);
        RegQueryValueEx(hKey, L"UnlockFpsOnFocus", NULL, NULL, (LPBYTE)&unlockFpsOnFocus, &dataSize);
        RegQueryValueEx(hKey, L"MultiInstanceInterval", NULL, NULL, (LPBYTE)&multiInstanceInterval, &dataSize);
        RegQueryValueEx(hKey, L"WindowOpacity", NULL, NULL, (LPBYTE)&windowOpacity, &dataSize);
        RegQueryValueEx(hKey, L"AfkReminder", NULL, NULL, (LPBYTE)&afkReminder, &dataSize);
        RegQueryValueEx(hKey, L"DoNotSleep", NULL, NULL, (LPBYTE)&doNotSleep, &dataSize);
        RegQueryValueEx(hKey, L"AutoMute", NULL, NULL, (LPBYTE)&autoMute, &dataSize);
        RegQueryValueEx(hKey, L"UnmuteOnFocus", NULL, NULL, (LPBYTE)&unmuteOnFocus, &dataSize);
        RegQueryValueEx(hKey, L"DiscordWebhookEnabled", NULL, NULL, (LPBYTE)&discordWebhookEnabled, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyStart", NULL, NULL, (LPBYTE)&discordNotifyStart, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyStop", NULL, NULL, (LPBYTE)&discordNotifyStop, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyAction", NULL, NULL, (LPBYTE)&discordNotifyAction, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyReconnect", NULL, NULL, (LPBYTE)&discordNotifyReconnect, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyReset", NULL, NULL, (LPBYTE)&discordNotifyReset, &dataSize);
        RegQueryValueEx(hKey, L"DiscordNotifyErrors", NULL, NULL, (LPBYTE)&discordNotifyErrors, &dataSize);
        RegQueryValueEx(hKey, L"DiscordDisableEmbed", NULL, NULL, (LPBYTE)&discordDisableEmbed, &dataSize);
        RegQueryValueEx(hKey, L"DiscordMentionOnErrors", NULL, NULL, (LPBYTE)&discordMentionOnErrors, &dataSize);

        DWORD webhookUrlSize = 0;
        if (RegQueryValueExW(hKey, L"DiscordWebhookUrl", NULL, NULL, NULL, &webhookUrlSize) == ERROR_SUCCESS && webhookUrlSize >= sizeof(wchar_t))
        {
            std::wstring urlBuffer(webhookUrlSize / sizeof(wchar_t), L'\0');
            if (RegQueryValueExW(hKey, L"DiscordWebhookUrl", NULL, NULL, (LPBYTE)&urlBuffer[0], &webhookUrlSize) == ERROR_SUCCESS)
            {
                if (!urlBuffer.empty() && urlBuffer.back() == L'\0')
                {
                    urlBuffer.pop_back();
                }
                discordWebhookUrl = NormalizeDiscordWebhookUrl(urlBuffer);
            }
        }
        RegCloseKey(hKey);
    }

    g_multiSupport = (multiSupport != 0);
    g_fishstrapSupport = (fishstrapSupport != 0);
    g_selectedTime = NormalizeSelectedTimeValue((int)selectedTime);
    g_selectedAction = NormalizeSelectedAction((int)selectedAction);
    g_autoUpdate = (autoUpdate != 0);
    g_userSafeMode = NormalizeUserSafeMode((int)userSafeMode);
    g_autoStartAfk = (autoStartAfk != 0);
    g_autoReconnect = (autoReconnect != 0);
    g_autoReset = (autoReset != 0);
    g_autoHideRoblox = (autoHideRoblox != 0);
    g_autoOpacity = (autoOpacity != 0);
    g_autoGrid = (autoGrid != 0);
    g_restoreMethod = NormalizeRestoreMethod((int)restoreMethod);
    g_tutorialShown = (tutorialShown != 0);
    g_totalAfkTimeSeconds = totalAfkTime;
    g_afkActionsPerformed = afkActions;
    g_autoReconnectsPerformed = autoReconnects;
    g_longestAfkSessionSeconds = longestAfkSession;
    g_discordWebhooksSent = discordWebhooksSent;
    g_programLaunches = programLaunches;
    g_afkSessionsCompleted = afkSessionsCompleted;
    g_useLegacyUi = (useLegacyUi != 0);
    g_bloxstrapIntegration = (bloxstrapIntegration != 0);
    g_statusBarEnabled = (statusBarEnabled != 0);
    g_fpsLimit = NormalizeFpsLimitValue((int)fpsLimit);
    g_unlockFpsOnFocus = (unlockFpsOnFocus != 0);
    g_multiInstanceInterval = NormalizeMultiInstanceIntervalValue((int)multiInstanceInterval);
    g_windowOpacity = (windowOpacity != 0);
    g_afkReminderEnabled = (afkReminder != 0);
    g_doNotSleep = (doNotSleep != 0);
    g_autoMute = (autoMute != 0);
    g_unmuteOnFocus = (unmuteOnFocus != 0);
    g_discordWebhookEnabled = (discordWebhookEnabled != 0);
    g_discordNotifyStart = (discordNotifyStart != 0) || (discordNotifyStop != 0);
    g_discordNotifyStop = g_discordNotifyStart.load();
    g_discordNotifyAction = (discordNotifyAction != 0);
    g_discordNotifyReconnect = (discordNotifyReconnect != 0);
    g_discordNotifyReset = (discordNotifyReset != 0);
    g_discordNotifyErrors = (discordNotifyErrors != 0);
    g_discordDisableEmbed = (discordDisableEmbed != 0);
    g_discordMentionOnErrors = (discordMentionOnErrors != 0);
    SetDiscordWebhookUrl(discordWebhookUrl);
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
    g_autoReset = false;
    g_autoHideRoblox = false;
    g_autoOpacity = false;
    g_autoGrid = false;
    g_restoreMethod = 1;
    g_tutorialShown = false;
    g_totalAfkTimeSeconds = 0;
    g_afkActionsPerformed = 0;
    g_autoReconnectsPerformed = 0;
    g_longestAfkSessionSeconds = 0;
    g_discordWebhooksSent = 0;
    g_programLaunches = 0;
    g_afkSessionsCompleted = 0;
    g_manuallyStoppedPids.clear();
    g_useLegacyUi = false;
    g_bloxstrapIntegration = false;
    g_statusBarEnabled = true;
    g_fpsLimit = 0;
    g_unlockFpsOnFocus = false;
    g_multiInstanceInterval = 0;
    g_windowOpacity = false;
    g_afkReminderEnabled = false;
    g_afkReminderState = 0;
    g_doNotSleep = false;
    SetThreadExecutionState(ES_CONTINUOUS);
    g_autoMute = false;
    g_unmuteOnFocus = false;
    g_unmutedPid = 0;
    g_discordWebhookEnabled = false;
    g_discordNotifyStart = true;
    g_discordNotifyStop = true;
    g_discordNotifyAction = false;
    g_discordNotifyReconnect = true;
    g_discordNotifyReset = false;
    g_discordNotifyErrors = true;
    g_discordDisableEmbed = false;
    g_discordMentionOnErrors = false;
    SetDiscordWebhookUrl(L"");
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

struct SettingsSnapshot {
    bool multiSupport = false;
    bool fishstrapSupport = false;
    int selectedTime = 540;
    int selectedAction = 0;
    bool autoUpdate = true;
    int userSafeMode = 0;
    bool autoStartAfk = false;
    bool autoReconnect = false;
    bool autoReset = false;
    bool autoHideRoblox = false;
    bool autoOpacity = false;
    bool autoGrid = false;
    int restoreMethod = 1;
    bool tutorialShown = false;
    uint64_t totalAfkTime = 0;
    uint64_t afkActions = 0;
    uint64_t autoReconnects = 0;
    uint64_t longestAfkSession = 0;
    uint64_t discordWebhooksSent = 0;
    uint64_t programLaunches = 0;
    uint64_t afkSessionsCompleted = 0;
    bool useLegacyUi = false;
    bool bloxstrapIntegration = false;
    bool statusBarEnabled = true;
    int fpsLimit = 0;
    bool unlockFpsOnFocus = false;
    int multiInstanceInterval = 0;
    bool windowOpacity = false;
    bool afkReminder = false;
    bool doNotSleep = false;
    bool autoMute = false;
    bool unmuteOnFocus = false;
    bool discordWebhookEnabled = false;
    bool discordNotifyStart = true;
    bool discordNotifyStop = true;
    bool discordNotifyAction = false;
    bool discordNotifyReconnect = true;
    bool discordNotifyReset = false;
    bool discordNotifyErrors = true;
    bool discordDisableEmbed = false;
    bool discordMentionOnErrors = false;
    std::wstring discordWebhookUrl;
};

SettingsSnapshot CaptureSettingsSnapshot()
{
    SettingsSnapshot s;
    s.multiSupport = g_multiSupport.load();
    s.fishstrapSupport = g_fishstrapSupport.load();
    s.selectedTime = g_selectedTime;
    s.selectedAction = g_selectedAction;
    s.autoUpdate = g_autoUpdate.load();
    s.userSafeMode = g_userSafeMode;
    s.autoStartAfk = g_autoStartAfk.load();
    s.autoReconnect = g_autoReconnect.load();
    s.autoReset = g_autoReset.load();
    s.autoHideRoblox = g_autoHideRoblox.load();
    s.autoOpacity = g_autoOpacity.load();
    s.autoGrid = g_autoGrid.load();
    s.restoreMethod = g_restoreMethod;
    s.tutorialShown = g_tutorialShown.load();
    s.totalAfkTime = g_totalAfkTimeSeconds.load();
    s.afkActions = g_afkActionsPerformed.load();
    s.autoReconnects = g_autoReconnectsPerformed.load();
    s.longestAfkSession = g_longestAfkSessionSeconds.load();
    s.discordWebhooksSent = g_discordWebhooksSent.load();
    s.programLaunches = g_programLaunches.load();
    s.afkSessionsCompleted = g_afkSessionsCompleted.load();
    s.useLegacyUi = g_useLegacyUi.load();
    s.bloxstrapIntegration = g_bloxstrapIntegration.load();
    s.statusBarEnabled = g_statusBarEnabled.load();
    s.fpsLimit = g_fpsLimit;
    s.unlockFpsOnFocus = g_unlockFpsOnFocus.load();
    s.multiInstanceInterval = g_multiInstanceInterval;
    s.windowOpacity = g_windowOpacity.load();
    s.afkReminder = g_afkReminderEnabled.load();
    s.doNotSleep = g_doNotSleep.load();
    s.autoMute = g_autoMute.load();
    s.unmuteOnFocus = g_unmuteOnFocus.load();
    s.discordWebhookEnabled = g_discordWebhookEnabled.load();
    s.discordNotifyStart = g_discordNotifyStart.load();
    s.discordNotifyStop = g_discordNotifyStop.load();
    s.discordNotifyAction = g_discordNotifyAction.load();
    s.discordNotifyReconnect = g_discordNotifyReconnect.load();
    s.discordNotifyReset = g_discordNotifyReset.load();
    s.discordNotifyErrors = g_discordNotifyErrors.load();
    s.discordDisableEmbed = g_discordDisableEmbed.load();
    s.discordMentionOnErrors = g_discordMentionOnErrors.load();
    s.discordWebhookUrl = GetDiscordWebhookUrlCopy();
    return s;
}

static std::wstring JsonEscape(const std::wstring& input)
{
    std::wstring out;
    out.reserve(input.size() + 8);
    for (wchar_t ch : input) {
        switch (ch) {
        case L'\\': out += L"\\\\"; break;
        case L'\"': out += L"\\\""; break;
        case L'\b': out += L"\\b"; break;
        case L'\f': out += L"\\f"; break;
        case L'\n': out += L"\\n"; break;
        case L'\r': out += L"\\r"; break;
        case L'\t': out += L"\\t"; break;
        default: out.push_back(ch); break;
        }
    }
    return out;
}

static void AppendJsonBool(std::wstringstream& ss, const wchar_t* key, bool value, bool& first)
{
    ss << (first ? L"" : L",\r\n");
    first = false;
    ss << L"  \"" << key << L"\": " << (value ? L"true" : L"false");
}

static void AppendJsonInt(std::wstringstream& ss, const wchar_t* key, int value, bool& first)
{
    ss << (first ? L"" : L",\r\n");
    first = false;
    ss << L"  \"" << key << L"\": " << value;
}

static void AppendJsonUInt64(std::wstringstream& ss, const wchar_t* key, uint64_t value, bool& first)
{
    ss << (first ? L"" : L",\r\n");
    first = false;
    ss << L"  \"" << key << L"\": " << value;
}

static std::wstring BuildSettingsJson(const SettingsSnapshot& s)
{
    std::wstringstream ss;
    ss << L"{\r\n";
    bool first = true;
    AppendJsonBool(ss, L"MultiSupport", s.multiSupport, first);
    AppendJsonBool(ss, L"FishstrapSupport", s.fishstrapSupport, first);
    AppendJsonInt(ss, L"SelectedTime", s.selectedTime, first);
    AppendJsonInt(ss, L"SelectedAction", s.selectedAction, first);
    AppendJsonBool(ss, L"AutoUpdate", s.autoUpdate, first);
    AppendJsonInt(ss, L"UserSafeMode", s.userSafeMode, first);
    AppendJsonBool(ss, L"AutoStartAfk", s.autoStartAfk, first);
    AppendJsonBool(ss, L"AutoReconnect", s.autoReconnect, first);
    AppendJsonBool(ss, L"AutoReset", s.autoReset, first);
    AppendJsonBool(ss, L"AutoHideRoblox", s.autoHideRoblox, first);
    AppendJsonBool(ss, L"AutoOpacity", s.autoOpacity, first);
    AppendJsonBool(ss, L"AutoGrid", s.autoGrid, first);
    AppendJsonInt(ss, L"RestoreMethod", s.restoreMethod, first);
    AppendJsonBool(ss, L"TutorialShown", s.tutorialShown, first);
    AppendJsonUInt64(ss, L"TotalAfkTime", s.totalAfkTime, first);
    AppendJsonUInt64(ss, L"AfkActions", s.afkActions, first);
    AppendJsonUInt64(ss, L"AutoReconnects", s.autoReconnects, first);
    AppendJsonUInt64(ss, L"LongestAfkSession", s.longestAfkSession, first);
    AppendJsonUInt64(ss, L"DiscordWebhooksSent", s.discordWebhooksSent, first);
    AppendJsonUInt64(ss, L"ProgramLaunches", s.programLaunches, first);
    AppendJsonUInt64(ss, L"AfkSessionsCompleted", s.afkSessionsCompleted, first);
    AppendJsonBool(ss, L"UseLegacyUI", s.useLegacyUi, first);
    AppendJsonBool(ss, L"BloxstrapIntegration", s.bloxstrapIntegration, first);
    AppendJsonBool(ss, L"StatusBarEnabled", s.statusBarEnabled, first);
    AppendJsonInt(ss, L"FpsLimit", s.fpsLimit, first);
    AppendJsonBool(ss, L"UnlockFpsOnFocus", s.unlockFpsOnFocus, first);
    AppendJsonInt(ss, L"MultiInstanceInterval", s.multiInstanceInterval, first);
    AppendJsonBool(ss, L"WindowOpacity", s.windowOpacity, first);
    AppendJsonBool(ss, L"AfkReminder", s.afkReminder, first);
    AppendJsonBool(ss, L"DoNotSleep", s.doNotSleep, first);
    AppendJsonBool(ss, L"AutoMute", s.autoMute, first);
    AppendJsonBool(ss, L"UnmuteOnFocus", s.unmuteOnFocus, first);
    AppendJsonBool(ss, L"DiscordWebhookEnabled", s.discordWebhookEnabled, first);
    AppendJsonBool(ss, L"DiscordNotifyStart", s.discordNotifyStart, first);
    AppendJsonBool(ss, L"DiscordNotifyStop", s.discordNotifyStop, first);
    AppendJsonBool(ss, L"DiscordNotifyAction", s.discordNotifyAction, first);
    AppendJsonBool(ss, L"DiscordNotifyReconnect", s.discordNotifyReconnect, first);
    AppendJsonBool(ss, L"DiscordNotifyReset", s.discordNotifyReset, first);
    AppendJsonBool(ss, L"DiscordNotifyErrors", s.discordNotifyErrors, first);
    AppendJsonBool(ss, L"DiscordDisableEmbed", s.discordDisableEmbed, first);
    AppendJsonBool(ss, L"DiscordMentionOnErrors", s.discordMentionOnErrors, first);
    ss << (first ? L"" : L",\r\n");
    ss << L"  \"DiscordWebhookUrl\": \"" << JsonEscape(s.discordWebhookUrl) << L"\"\r\n";
    ss << L"}\r\n";
    return ss.str();
}

static bool SaveTextFileUtf8(const std::wstring& path, const std::wstring& content)
{
    std::ofstream outFile(path, std::ios::binary);
    if (!outFile.is_open()) return false;
    int bytesNeeded = WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, NULL, 0, NULL, NULL);
    if (bytesNeeded <= 0) return false;
    std::string utf8(bytesNeeded - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, &utf8[0], bytesNeeded, NULL, NULL);
    outFile.write(utf8.data(), (std::streamsize)utf8.size());
    return outFile.good();
}

static bool LoadTextFileUtf8(const std::wstring& path, std::wstring& content)
{
    std::ifstream inFile(path, std::ios::binary);
    if (!inFile.is_open()) return false;
    std::string bytes((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    if (bytes.empty()) return false;
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, bytes.c_str(), (int)bytes.size(), NULL, 0);
    if (charsNeeded <= 0) return false;
    content.resize(charsNeeded);
    MultiByteToWideChar(CP_UTF8, 0, bytes.c_str(), (int)bytes.size(), &content[0], charsNeeded);
    return true;
}

static bool FindJsonValue(const std::wstring& json, const std::wstring& key, std::wstring& rawValue)
{
    size_t pos = json.find(L"\"" + key + L"\"");
    if (pos == std::wstring::npos) return false;
    pos = json.find(L':', pos);
    if (pos == std::wstring::npos) return false;
    ++pos;
    while (pos < json.size() && iswspace(json[pos])) ++pos;
    size_t end = pos;
    if (pos < json.size() && json[pos] == L'"') {
        ++pos;
        end = pos;
        bool escape = false;
        while (end < json.size()) {
            if (!escape && json[end] == L'"') break;
            escape = (!escape && json[end] == L'\\');
            if (escape && json[end] != L'\\') escape = false;
            ++end;
        }
        if (end >= json.size()) return false;
        rawValue = json.substr(pos, end - pos);
        return true;
    }
    while (end < json.size() && json[end] != L',' && json[end] != L'}' && json[end] != L'\r' && json[end] != L'\n') ++end;
    rawValue = json.substr(pos, end - pos);
    rawValue.erase(0, rawValue.find_first_not_of(L" \t"));
    rawValue.erase(rawValue.find_last_not_of(L" \t") + 1);
    return !rawValue.empty();
}

static bool ParseJsonBool(const std::wstring& json, const std::wstring& key, bool& value)
{
    std::wstring raw;
    if (!FindJsonValue(json, key, raw)) return false;
    for (auto& ch : raw) ch = (wchar_t)towlower(ch);
    if (raw == L"true" || raw == L"1") { value = true; return true; }
    if (raw == L"false" || raw == L"0") { value = false; return true; }
    return false;
}

static bool ParseJsonInt(const std::wstring& json, const std::wstring& key, int& value)
{
    std::wstring raw;
    if (!FindJsonValue(json, key, raw)) return false;
    try { value = std::stoi(raw); return true; } catch (...) { return false; }
}

static bool ParseJsonUInt64(const std::wstring& json, const std::wstring& key, uint64_t& value)
{
    std::wstring raw;
    if (!FindJsonValue(json, key, raw)) return false;
    try { value = std::stoull(raw); return true; } catch (...) { return false; }
}

static bool ParseJsonString(const std::wstring& json, const std::wstring& key, std::wstring& value)
{
    std::wstring raw;
    if (!FindJsonValue(json, key, raw)) return false;
    if (raw.empty()) {
        value.clear();
        return true;
    }
    std::wstring out;
    out.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == L'\\' && i + 1 < raw.size()) {
            wchar_t next = raw[++i];
            switch (next) {
            case L'\\': out.push_back(L'\\'); break;
            case L'"': out.push_back(L'"'); break;
            case L'n': out.push_back(L'\n'); break;
            case L'r': out.push_back(L'\r'); break;
            case L't': out.push_back(L'\t'); break;
            case L'b': out.push_back(L'\b'); break;
            case L'f': out.push_back(L'\f'); break;
            default: out.push_back(next); break;
            }
        } else {
            out.push_back(raw[i]);
        }
    }
    value = out;
    return true;
}

static void ApplySettingsSnapshot(const SettingsSnapshot& s)
{
    g_multiSupport = s.multiSupport;
    g_fishstrapSupport = s.fishstrapSupport;
    g_selectedTime = NormalizeSelectedTimeValue(s.selectedTime);
    g_selectedAction = NormalizeSelectedAction(s.selectedAction);
    g_autoUpdate = s.autoUpdate;
    g_userSafeMode = NormalizeUserSafeMode(s.userSafeMode);
    g_autoStartAfk = s.autoStartAfk;
    g_autoReconnect = s.autoReconnect;
    g_autoReset = s.autoReset;
    g_autoHideRoblox = s.autoHideRoblox;
    g_autoOpacity = s.autoOpacity;
    g_autoGrid = s.autoGrid;
    g_restoreMethod = NormalizeRestoreMethod(s.restoreMethod);
    g_tutorialShown = s.tutorialShown;
    g_totalAfkTimeSeconds = s.totalAfkTime;
    g_afkActionsPerformed = s.afkActions;
    g_autoReconnectsPerformed = s.autoReconnects;
    g_longestAfkSessionSeconds = s.longestAfkSession;
    g_discordWebhooksSent = s.discordWebhooksSent;
    g_programLaunches = s.programLaunches;
    g_afkSessionsCompleted = s.afkSessionsCompleted;
    g_useLegacyUi = s.useLegacyUi;
    g_bloxstrapIntegration = s.bloxstrapIntegration;
    g_statusBarEnabled = s.statusBarEnabled;
    g_fpsLimit = NormalizeFpsLimitValue(s.fpsLimit);
    g_unlockFpsOnFocus = s.unlockFpsOnFocus;
    g_multiInstanceInterval = NormalizeMultiInstanceIntervalValue(s.multiInstanceInterval);
    g_windowOpacity = s.windowOpacity;
    g_afkReminderEnabled = s.afkReminder;
    g_doNotSleep = s.doNotSleep;
    g_autoMute = s.autoMute;
    g_unmuteOnFocus = s.unmuteOnFocus;
    g_discordWebhookEnabled = s.discordWebhookEnabled;
    g_discordNotifyStart = s.discordNotifyStart || s.discordNotifyStop;
    g_discordNotifyStop = g_discordNotifyStart.load();
    g_discordNotifyAction = s.discordNotifyAction;
    g_discordNotifyReconnect = s.discordNotifyReconnect;
    g_discordNotifyReset = s.discordNotifyReset;
    g_discordNotifyErrors = s.discordNotifyErrors;
    g_discordDisableEmbed = s.discordDisableEmbed;
    g_discordMentionOnErrors = s.discordMentionOnErrors;
    SetDiscordWebhookUrl(s.discordWebhookUrl);
}

static bool ExportSettingsToFile(HWND owner)
{
    wchar_t fileName[MAX_PATH] = L"AntiAFK-RBX-settings.json";
    OPENFILENAME ofn = { sizeof(ofn) };
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"JSON files (*.json)\0*.json\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"json";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    if (!GetSaveFileName(&ofn)) return false;
    return SaveTextFileUtf8(fileName, BuildSettingsJson(CaptureSettingsSnapshot()));
}

static bool ImportSettingsFromFile(HWND owner)
{
    wchar_t fileName[MAX_PATH] = L"";
    OPENFILENAME ofn = { sizeof(ofn) };
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"JSON files (*.json)\0*.json\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    if (!GetOpenFileName(&ofn)) return false;

    std::wstring json;
    if (!LoadTextFileUtf8(fileName, json)) return false;

    SettingsSnapshot s = CaptureSettingsSnapshot();
    ParseJsonBool(json, L"MultiSupport", s.multiSupport);
    ParseJsonBool(json, L"FishstrapSupport", s.fishstrapSupport);
    ParseJsonInt(json, L"SelectedTime", s.selectedTime);
    ParseJsonInt(json, L"SelectedAction", s.selectedAction);
    ParseJsonBool(json, L"AutoUpdate", s.autoUpdate);
    ParseJsonInt(json, L"UserSafeMode", s.userSafeMode);
    ParseJsonBool(json, L"AutoStartAfk", s.autoStartAfk);
    ParseJsonBool(json, L"AutoReconnect", s.autoReconnect);
    ParseJsonBool(json, L"AutoReset", s.autoReset);
    ParseJsonBool(json, L"AutoHideRoblox", s.autoHideRoblox);
    ParseJsonBool(json, L"AutoOpacity", s.autoOpacity);
    ParseJsonBool(json, L"AutoGrid", s.autoGrid);
    ParseJsonInt(json, L"RestoreMethod", s.restoreMethod);
    ParseJsonBool(json, L"TutorialShown", s.tutorialShown);
    ParseJsonUInt64(json, L"TotalAfkTime", s.totalAfkTime);
    ParseJsonUInt64(json, L"AfkActions", s.afkActions);
    ParseJsonUInt64(json, L"AutoReconnects", s.autoReconnects);
    ParseJsonUInt64(json, L"LongestAfkSession", s.longestAfkSession);
    ParseJsonUInt64(json, L"DiscordWebhooksSent", s.discordWebhooksSent);
    ParseJsonUInt64(json, L"ProgramLaunches", s.programLaunches);
    ParseJsonUInt64(json, L"AfkSessionsCompleted", s.afkSessionsCompleted);
    ParseJsonBool(json, L"UseLegacyUI", s.useLegacyUi);
    ParseJsonBool(json, L"BloxstrapIntegration", s.bloxstrapIntegration);
    ParseJsonBool(json, L"StatusBarEnabled", s.statusBarEnabled);
    ParseJsonInt(json, L"FpsLimit", s.fpsLimit);
    ParseJsonBool(json, L"UnlockFpsOnFocus", s.unlockFpsOnFocus);
    ParseJsonInt(json, L"MultiInstanceInterval", s.multiInstanceInterval);
    ParseJsonBool(json, L"WindowOpacity", s.windowOpacity);
    ParseJsonBool(json, L"AfkReminder", s.afkReminder);
    ParseJsonBool(json, L"DoNotSleep", s.doNotSleep);
    ParseJsonBool(json, L"AutoMute", s.autoMute);
    ParseJsonBool(json, L"UnmuteOnFocus", s.unmuteOnFocus);
    ParseJsonBool(json, L"DiscordWebhookEnabled", s.discordWebhookEnabled);
    ParseJsonBool(json, L"DiscordNotifyStart", s.discordNotifyStart);
    ParseJsonBool(json, L"DiscordNotifyStop", s.discordNotifyStop);
    ParseJsonBool(json, L"DiscordNotifyAction", s.discordNotifyAction);
    ParseJsonBool(json, L"DiscordNotifyReconnect", s.discordNotifyReconnect);
    ParseJsonBool(json, L"DiscordNotifyReset", s.discordNotifyReset);
    ParseJsonBool(json, L"DiscordNotifyErrors", s.discordNotifyErrors);
    ParseJsonBool(json, L"DiscordDisableEmbed", s.discordDisableEmbed);
    ParseJsonBool(json, L"DiscordMentionOnErrors", s.discordMentionOnErrors);
    ParseJsonString(json, L"DiscordWebhookUrl", s.discordWebhookUrl);
    ApplySettingsSnapshot(s);
    SaveSettings();
    return true;
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
        AppendMenu(g_hMenu, MF_STRING | (g_windowOpacity.load() ? MF_CHECKED : 0), ID_WINDOW_OPACITY, L"Window Opacity (70%)");
        AppendMenu(g_hMenu, MF_STRING, ID_GRID_SNAP, L"Grid Snap Roblox*");
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING, ID_OPEN_UI, L"Open AntiAFK-RBX");
        AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(g_hMenu, MF_STRING | (g_useLegacyUi.load() ? MF_CHECKED : 0), ID_USE_LEGACY_UI, L"Use Legacy UI (Tray)");
        if (g_updateFound)
            AppendMenu(g_hMenu, MF_STRING, ID_UPDATE_AVAILABLE, L"[⤓] • Update Available");
        AppendMenu(g_hMenu, MF_STRING, ID_EXIT, L"Exit");
        return;
    }

    wchar_t infoText[128];
    swprintf_s(infoText, L"Legacy UI • %s", g_Version);
    AppendMenu(g_hMenu, MF_STRING | MF_GRAYED, ID_INFORMATION, infoText);
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, ID_EMULATE_SPACE, L"Test Anti-AFK move");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_GRAYED : MF_STRING), ID_START_AFK, L"Start Anti-AFK");
    AppendMenu(g_hMenu, MF_STRING | (afk ? MF_STRING : MF_GRAYED), ID_STOP_AFK, L"Stop Anti-AFK");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);

    HMENU hTimeSubmenu = CreatePopupMenu();
    AppendMenu(hTimeSubmenu, MF_STRING, ID_TIME_CUSTOM, L"Custom interval...");
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
    AppendMenu(g_hMenu, MF_STRING | (g_windowOpacity.load() ? MF_CHECKED : 0), ID_WINDOW_OPACITY, L"Window Opacity (70%)");
    AppendMenu(g_hMenu, MF_STRING, ID_GRID_SNAP, L"Grid Snap Roblox*");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    HMENU hSettingsSubmenu = CreatePopupMenu();
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_fishstrapSupport.load() ? MF_CHECKED : 0), ID_FISHSTRAP_SUP, L"FishStrap/Shaders Support");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoUpdate.load() ? MF_CHECKED : 0), ID_AUTO_UPDATE, L"Update Checker");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_statusBarEnabled.load() ? MF_CHECKED : 0), ID_STATUS_BAR, L"Status Bar*");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoStartAfk.load() ? MF_CHECKED : 0), ID_AUTO_START_AFK, L"Auto-Start AntiAFK");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoReconnect.load() ? MF_CHECKED : 0), ID_AUTO_RECONNECT, L"Auto Reconnect*");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoReset.load() ? MF_CHECKED : 0), ID_AUTO_RESET, L"Auto Reset (Esc+R+Enter)");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoHideRoblox.load() ? MF_CHECKED : 0), ID_AUTO_HIDE, L"Auto-Hide Roblox");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoOpacity.load() ? MF_CHECKED : 0), ID_AUTO_OPACITY, L"Auto Opacity (70%)");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoGrid.load() ? MF_CHECKED : 0), ID_AUTO_GRID, L"Auto Grid Roblox*");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_bloxstrapIntegration.load() ? MF_CHECKED : 0), ID_BLOXSTRAP_INTEGRATION, L"Fish/Void/Bloxstrap Integration");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_unlockFpsOnFocus.load() ? MF_CHECKED : 0), ID_UNLOCK_FPS_ON_FOCUS, L"Unlock FPS when focus");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_afkReminderEnabled.load() ? MF_CHECKED : 0), ID_I_CAN_FORGET, L"I can forget (smart auto-start)");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_doNotSleep.load() ? MF_CHECKED : 0), ID_DO_NOT_SLEEP, L"Do not sleep");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_autoMute.load() ? MF_CHECKED : 0), ID_AUTO_MUTE, L"AutoMute Roblox");
    AppendMenu(hSettingsSubmenu, MF_STRING | (g_unmuteOnFocus.load() ? MF_CHECKED : 0), ID_UNMUTE_ON_FOCUS, L"Unmute on focus");

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


    AppendMenu(hSettingsSubmenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSettingsSubmenu, MF_STRING, ID_IMPORT_SETTINGS, L"Import Settings...");
    AppendMenu(hSettingsSubmenu, MF_STRING, ID_EXPORT_SETTINGS, L"Export Settings...");
    AppendMenu(hSettingsSubmenu, MF_STRING, ID_RESET_SETTINGS, L"Reset Settings");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSettingsSubmenu, L"Settings");

    HMENU hUtilsSubmenu = CreatePopupMenu();
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_UTILS_SHOW_ALL, L"Show All Roblox");
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_UTILS_HIDE_ALL, L"Hide All Roblox");
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_GRID_SNAP, L"Grid Snap Roblox*");
    AppendMenu(hUtilsSubmenu, MF_STRING | (IsUtilsSessionWindowOpacityEnabled() ? MF_CHECKED : 0), ID_UTILS_TOGGLE_WINDOW_OPACITY, L"Session Window Opacity");
    AppendMenu(hUtilsSubmenu, MF_STRING | (IsUtilsSessionMuteEnabled() ? MF_CHECKED : 0), ID_UTILS_TOGGLE_MUTE, L"Session Mute Roblox");
    AppendMenu(hUtilsSubmenu, MF_STRING | (IsUtilsSessionFpsLimitEnabled() ? MF_CHECKED : 0), ID_UTILS_TOGGLE_FPS, L"Session FPS Limit*");
    AppendMenu(hUtilsSubmenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_UTILS_TEST_ACTION, L"Test AntiAFK Action");
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_UTILS_RESET_ALL, L"Reset All Roblox");
    AppendMenu(hUtilsSubmenu, MF_STRING, ID_UTILS_CLOSE_ALL, L"Close All Roblox");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hUtilsSubmenu, L"Utils");

    HMENU hStatsSubmenu = CreatePopupMenu();
    {
        wchar_t statText[128];
        std::wstring totalAfk = FormatDurationShort(g_totalAfkTimeSeconds.load());
        swprintf_s(statText, L"Total AFK Time • %s", totalAfk.c_str());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        swprintf_s(statText, L"AFK Actions • %llu", g_afkActionsPerformed.load());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        swprintf_s(statText, L"Auto Reconnects • %llu", g_autoReconnectsPerformed.load());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        swprintf_s(statText, L"Webhook Events • %llu", g_discordWebhooksSent.load());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        swprintf_s(statText, L"Sessions Completed • %llu", g_afkSessionsCompleted.load());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        swprintf_s(statText, L"Program Launches • %llu", g_programLaunches.load());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
        std::wstring longestAfk = FormatDurationShort(g_longestAfkSessionSeconds.load());
        swprintf_s(statText, L"Longest Session • %s", longestAfk.c_str());
        AppendMenu(hStatsSubmenu, MF_STRING | MF_GRAYED, 0, statText);
    }
    AppendMenu(hStatsSubmenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hStatsSubmenu, MF_STRING, ID_RESET_STATS, L"Reset Statistics");
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hStatsSubmenu, L"Statistics");

    HMENU hDiscordSubmenu = CreatePopupMenu();
    {
        const std::wstring webhookUrl = GetDiscordWebhookUrlCopy();
        const bool webhookConfigured = IsDiscordWebhookUrl(webhookUrl);
        AppendMenu(hDiscordSubmenu, MF_STRING | MF_GRAYED, 0, webhookConfigured ? L"Webhook URL • Configured" : L"Webhook URL • Not set");
        AppendMenu(hDiscordSubmenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordWebhookEnabled.load() ? MF_CHECKED : 0), ID_DISCORD_WEBHOOK_ENABLE, L"Enable Webhook");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordNotifyStart.load() ? MF_CHECKED : 0), ID_DISCORD_NOTIFY_START, L"Notify Start / Stop");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordNotifyAction.load() ? MF_CHECKED : 0), ID_DISCORD_NOTIFY_ACTION, L"Notify Action");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordNotifyReconnect.load() ? MF_CHECKED : 0), ID_DISCORD_NOTIFY_RECONNECT, L"Notify Reconnect");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordNotifyErrors.load() ? MF_CHECKED : 0), ID_DISCORD_NOTIFY_ERRORS, L"Notify Errors");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordDisableEmbed.load() ? MF_CHECKED : 0), ID_DISCORD_DISABLE_EMBED, L"Disable Embed");
        AppendMenu(hDiscordSubmenu, MF_STRING | (g_discordMentionOnErrors.load() ? MF_CHECKED : 0), ID_DISCORD_MENTION_ON_ERRORS, L"Mention on Errors");
        AppendMenu(hDiscordSubmenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hDiscordSubmenu, MF_STRING, ID_DISCORD_WEBHOOK_PASTE, L"Paste URL from Clipboard");
        AppendMenu(hDiscordSubmenu, MF_STRING | (webhookUrl.empty() ? MF_GRAYED : 0), ID_DISCORD_WEBHOOK_CLEAR, L"Clear Webhook URL");
        AppendMenu(hDiscordSubmenu, MF_STRING | (webhookConfigured ? 0 : MF_GRAYED), ID_DISCORD_WEBHOOK_TEST, L"Send Test Webhook");
    }
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hDiscordSubmenu, L"Discord Webhook");

    AppendMenu(g_hMenu, MF_STRING | (g_useLegacyUi.load() ? MF_CHECKED : 0), ID_USE_LEGACY_UI, L"Use Legacy UI (Tray)");
    AppendMenu(g_hMenu, MF_STRING | (g_multiSupport.load() ? MF_CHECKED : 0), ID_MULTI_SUPPORT, L"Multi-Instance Support");
    
    HMENU hMiIntervalSubmenu = CreatePopupMenu();
    AppendMenu(hMiIntervalSubmenu, MF_STRING | (g_multiInstanceInterval == 0 ? MF_CHECKED : 0), ID_MI_INTERVAL_0, L"Minimum (0)");
    AppendMenu(hMiIntervalSubmenu, MF_STRING | (g_multiInstanceInterval == 1000 ? MF_CHECKED : 0), ID_MI_INTERVAL_1, L"1 sec");
    AppendMenu(hMiIntervalSubmenu, MF_STRING | (g_multiInstanceInterval == 3000 ? MF_CHECKED : 0), ID_MI_INTERVAL_3, L"3 sec");
    AppendMenu(hMiIntervalSubmenu, MF_STRING | (g_multiInstanceInterval == 5000 ? MF_CHECKED : 0), ID_MI_INTERVAL_5, L"5 sec");
    AppendMenu(hMiIntervalSubmenu, MF_STRING | (g_multiInstanceInterval == 10000 ? MF_CHECKED : 0), ID_MI_INTERVAL_10, L"10 sec");
    wchar_t miIntervalLabel[48];
    const wchar_t* miLabelV = g_multiInstanceInterval == 0 ? L"Min" : (g_multiInstanceInterval == 1000 ? L"1s" : (g_multiInstanceInterval == 3000 ? L"3s" : (g_multiInstanceInterval == 5000 ? L"5s" : L"10s")));
    swprintf_s(miIntervalLabel, L"Multi-Instance Interval • %s", miLabelV);
    AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hMiIntervalSubmenu, miIntervalLabel);
    
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    if (g_updateFound)
        AppendMenu(g_hMenu, MF_STRING, ID_UPDATE_AVAILABLE, L"[⤓] • Update Available");
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
    DWORD timeoutMs = 5000;
    g_updateFound = false;
    g_updateCheckFailed = false;
    HINTERNET hInternet = InternetOpen(L"AntiAFK-RBX/3.2", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        g_updateCheckFailed = true;
        return false;
    }

    InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    HINTERNET hConnect = InternetOpenUrl(hInternet, L"https://raw.githubusercontent.com/Agzes/AntiAFK-RBX/refs/heads/main/version", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hConnect) {
        g_updateCheckFailed = true;
        InternetCloseHandle(hInternet);
        return false;
    }

    InternetSetOption(hConnect, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOption(hConnect, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

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
            g_updateCheckFailed = true;
        }
        catch (const std::out_of_range& oor) {
            g_updateCheckFailed = true;
        }
    }
    else
    {
        g_updateCheckFailed = true;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return updateFound;
}

// Custom Interval Window
struct CustomIntervalData {
    HFONT hFontTitle = NULL, hFontText = NULL, hFontInput = NULL;
    RECT closeButtonRect = { 0 }, okButtonRect = { 0 }, cancelButtonRect = { 0 }, inputRect = { 0 };
    bool isHoveringClose = false, isHoveringOk = false, isHoveringCancel = false, isHoveringInput = false;
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

        enum DWM_WINDOW_CORNER_PREFERENCE { DWMWCP_ROUND = 2 };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);
        pData->hFontTitle = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontInput = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int inputMargin = 0, btnH = 30, btnGap = 0, inputH = 30, topBarH = 30, infoH = 23;

        pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, topBarH };

        int inputTop = 53;
        int inputBottom = inputTop + inputH;
        int btnY = inputBottom - 1;
        pData->inputRect = { inputMargin, inputTop, clientRect.right - inputMargin, inputBottom };
        int totalWidth = clientRect.right;
        int btnWidth = (totalWidth - btnGap) / 2;

        pData->cancelButtonRect = { 0, btnY, btnWidth, btnY + btnH };
        pData->okButtonRect = { btnWidth + btnGap, btnY, btnWidth + btnGap + btnWidth, btnY + btnH };

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
        if (hover_input != pData->isHoveringInput) { pData->isHoveringInput = hover_input; InvalidateRect(hwnd, &pData->inputRect, FALSE); }

        if (hover_ok || hover_cancel || hover_close) SetCursor(pData->hCursorHand);
        else if (hover_input) SetCursor(pData->hCursorText);
        else SetCursor(pData->hCursorArrow);

        if ((hover_ok || hover_cancel || hover_close || hover_input) && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringOk = pData->isHoveringCancel = pData->isHoveringClose = pData->isHoveringInput = false;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, &pData->okButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->inputRect, FALSE);
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

        Popup_DrawChrome(&g, memDC, clientRect, pData->closeButtonRect, pData->hFontTitle, L"Set Custom Interval", pData->isHoveringClose);

        RECT infoRect = { 0, 30, clientRect.right, 53 };
        Popup_DrawInfoBannerNoBottomBorder(&g, memDC, infoRect, pData->hFontText, L"\uE916", RGB(0, 122, 204), L"Enter interval in seconds. Maximum value is 1200.");
        Popup_DrawTextInputJoinedToButtons(&g, memDC, pData->inputRect, pData->hFontInput, pData->inputText, L"60", pData->isHoveringInput, pData->hasFocus);

        if (pData->hasFocus) {
             Font inputFont(memDC, pData->hFontInput);
             RectF size, layout((REAL)0, (REAL)0, (REAL)1000, (REAL)1000);
             g.MeasureString(pData->inputText, -1, &inputFont, layout, &size);
             
             int caretX = pData->inputRect.left + 12 + (int)size.Width;
             int caretY = pData->inputRect.top + 5;
             SetCaretPos(caretX, caretY);
        }

        Popup_DrawActionButtonWithoutTopBorder(&g, memDC, pData->cancelButtonRect, pData->hFontText, L"Cancel", pData->isHoveringCancel, false);
        Popup_DrawActionButtonWithoutTopBorder(&g, memDC, pData->okButtonRect, pData->hFontText, L"Apply", pData->isHoveringOk, true);

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

    const int dlgW = 344, dlgH = 112;
    RECT ownerRect = { 0 };
    int x = (GetSystemMetrics(SM_CXSCREEN) - dlgW) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - dlgH) / 2;
    if (owner) {
        GetWindowRect(owner, &ownerRect);
        x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dlgW) / 2;
        y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dlgH) / 2;
    }


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
    bool isHoveringClose = false, isHoveringOk = false, isHoveringCancel = false, isHoveringInput = false;
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
        pData->hFontTitle = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontInput = CreateFontW(-MulDiv(10, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const int inputMargin = 0, btnH = 30, btnGap = 0, inputH = 30, topBarH = 30, infoH = 23;

        pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, topBarH };

        int inputTop = 53;
        int inputBottom = inputTop + inputH;
        int btnY = inputBottom - 1;
        pData->inputRect = { inputMargin, inputTop, clientRect.right - inputMargin, inputBottom };
        int totalWidth = clientRect.right;
        int btnWidth = (totalWidth - btnGap) / 2;

        pData->cancelButtonRect = { 0, btnY, btnWidth, btnY + btnH };
        pData->okButtonRect = { btnWidth + btnGap, btnY, btnWidth + btnGap + btnWidth, btnY + btnH };

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
        if (hover_input != pData->isHoveringInput) { pData->isHoveringInput = hover_input; InvalidateRect(hwnd, &pData->inputRect, FALSE); }

        if (hover_ok || hover_cancel || hover_close) SetCursor(pData->hCursorHand);
        else if (hover_input) SetCursor(pData->hCursorText);
        else SetCursor(pData->hCursorArrow);

        if ((hover_ok || hover_cancel || hover_close || hover_input) && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringOk = pData->isHoveringCancel = pData->isHoveringClose = pData->isHoveringInput = false;
        pData->isTrackingMouse = false;
        InvalidateRect(hwnd, &pData->okButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->cancelButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->closeButtonRect, FALSE);
        InvalidateRect(hwnd, &pData->inputRect, FALSE);
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

        Popup_DrawChrome(&g, memDC, clientRect, pData->closeButtonRect, pData->hFontTitle, L"Set Custom FPS Limit", pData->isHoveringClose);

        RECT infoRect = { 0, 30, clientRect.right, 53 };
        Popup_DrawInfoBannerNoBottomBorder(&g, memDC, infoRect, pData->hFontText, L"\uF272", RGB(0, 122, 204), L"Set FPS cap from 0 to 60. Use 0 to turn it off.");
        Popup_DrawTextInputJoinedToButtons(&g, memDC, pData->inputRect, pData->hFontInput, pData->inputText, L"0", pData->isHoveringInput, pData->hasFocus);

        if (pData->hasFocus) {
            Font inputFont(memDC, pData->hFontInput);
            RectF size, layout((REAL)0, (REAL)0, (REAL)1000, (REAL)1000);
            g.MeasureString(pData->inputText, -1, &inputFont, layout, &size);
            
            int caretX = pData->inputRect.left + 12 + (int)size.Width;
            int caretY = pData->inputRect.top + 5;
            SetCaretPos(caretX, caretY);
        }

        Popup_DrawActionButtonWithoutTopBorder(&g, memDC, pData->cancelButtonRect, pData->hFontText, L"Cancel", pData->isHoveringCancel, false);
        Popup_DrawActionButtonWithoutTopBorder(&g, memDC, pData->okButtonRect, pData->hFontText, L"Apply", pData->isHoveringOk, true);

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

    const int dlgW = 344, dlgH = 112;
    RECT ownerRect = { 0 };
    int x = (GetSystemMetrics(SM_CXSCREEN) - dlgW) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - dlgH) / 2;
    if (owner) {
        GetWindowRect(owner, &ownerRect);
        x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dlgW) / 2;
        y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dlgH) / 2;
    }

    g_hCustomFpsWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW, CUSTOM_FPS_CLASS_NAME,
        L"Set Custom FPS Limit", WS_POPUP,
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
void MainUI_Paint_DrawToggle(HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool checked, bool isHovering, float animState, bool hasRightNeighbor, const wchar_t* icon);
void MainUI_Paint_DrawHelpButton(HDC hdc, const RECT& rect, HFONT font, bool isHovering);
struct TutorialData {
    HFONT hFont40 = NULL, hFont20 = NULL, hFont14 = NULL, hFont13 = NULL, hFont11 = NULL, hFont14b = NULL, hFontTitle = NULL;
    RECT nextButtonRect = { 0 }, backButtonRect = { 0 };
    RECT skipLinkRect = { 0 }, githubLinkRect = { 0 }, wikiLinkRect = { 0 }, closeButtonRect = { 0 };
    RECT demoHelpButtonRect = { 0 }, discordLinkRect = { 0 };
    bool isHoveringNext = false, isHoveringBack = false;
    bool isHoveringSkip = false, isHoveringGithub = false, isHoveringWiki = false, isHoveringClose = false;
    bool isHoveringDemoHelp = false, isHoveringDiscordLink = false;
    bool isTrackingMouse = false;
    int page = 0; // 0: welcome, 1: basics, 2: help, 3: workflow, 4: settings, 5: final
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
        pData->hFontTitle = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        const int dlgW = 472, dlgH = 312;
        const int btnH = 32, progressBarHeight = 4;
        int y_pos = dlgH - btnH - progressBarHeight;

        pData->backButtonRect = { 0, y_pos, dlgW / 2, y_pos + btnH };
        pData->skipLinkRect = { 0, y_pos, 96, y_pos + btnH };
        pData->nextButtonRect = { 96, y_pos, dlgW, y_pos + btnH };
        pData->githubLinkRect = { 0, 0, 0, 0 };
        pData->wikiLinkRect = { 0, 0, 0, 0 };
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
        if (pData->page == 2) anyHover |= checkHover(pData->isHoveringDemoHelp, pData->demoHelpButtonRect);
        if (pData->page == 5) anyHover |= checkHover(pData->isHoveringDiscordLink, pData->discordLinkRect);

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
        pData->isHoveringNext = pData->isHoveringBack = false;
        pData->isHoveringSkip = pData->isHoveringGithub = pData->isHoveringWiki = pData->isHoveringClose = false;
        pData->isHoveringDemoHelp = pData->isHoveringDiscordLink = false;
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
        if (pData->page == 2 && PtInRect(&pData->githubLinkRect, pt)) { PostMessage(hwnd, WM_COMMAND, ID_LINKGITHUB, 0); return 0; }
        if (pData->page == 2 && PtInRect(&pData->demoHelpButtonRect, pt)) {
            ShowDarkMessageBox(hwnd,
                L"These help buttons explain what a setting does, when to use it, and whether it is recommended.\n\nIf you are unsure, click the ? button next to a setting before changing it.",
                L"Tutorial Help Example",
                MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        if (pData->page == 5 && PtInRect(&pData->discordLinkRect, pt)) {
            ShellExecute(NULL, L"open", L"https://agzes.github.io/go/to/discord", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        }

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

        const wchar_t* nextText = (pData->page == 5) ? L"Got it!" : L"Next";
        const int btnH = 32;
        const int progressBarHeight = 4;
        const int btnTop = dlgH - btnH - progressBarHeight;

        if (pData->page == 0) {
            pData->skipLinkRect = { 0, btnTop, 96, btnTop + btnH };
            pData->nextButtonRect = { 96, btnTop, dlgW, btnTop + btnH };
            pData->backButtonRect = { 0, 0, 0, 0 };
        } else {
            pData->backButtonRect = { 0, btnTop, dlgW / 2, btnTop + btnH };
            pData->nextButtonRect = { dlgW / 2, btnTop, dlgW, btnTop + btnH };
            pData->skipLinkRect = { 0, 0, 0, 0 };
        }

        Graphics g(memDC);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        Popup_DrawChrome(&g, memDC, clientRect, pData->closeButtonRect, pData->hFontTitle, L"AntiAFK-RBX • Tutorial", pData->isHoveringClose);

        auto drawTextBlock = [&](const wchar_t* title, const wchar_t* body) {
            Font titleFont(memDC, pData->hFont20);
            Font bodyFont(memDC, pData->hFont13);
            SolidBrush titleBrush(Color(255, 255, 255, 255));
            SolidBrush bodyBrush(Color(255, 210, 210, 210));

            RectF titleRect(28.0f, 46.0f, (REAL)(clientRect.right - 56), 24.0f);
            RectF bodyRect(28.0f, 78.0f, (REAL)(clientRect.right - 56), 136.0f);
            g.DrawString(title, -1, &titleFont, titleRect, NULL, &titleBrush);
            g.DrawString(body, -1, &bodyFont, bodyRect, NULL, &bodyBrush);
        };

        if (pData->page == 0)
        {
            const int margin = 20;
            Font font20(memDC, pData->hFont20);
            SolidBrush grayBrush(Color(255, 180, 180, 180));
            int y_pos = 60;
            g.DrawString(L"Welcome to", -1, &font20, PointF((REAL)margin + 3, (REAL)y_pos + 2), &grayBrush);
            
            Font font40(memDC, pData->hFont40);
            const wchar_t* p1 = L"AntiAFK-";
            const wchar_t* p2 = L"RBX";
            
            RectF s1, s2;
            const StringFormat* typographic = StringFormat::GenericTypographic();
            g.MeasureString(p1, -1, &font40, PointF(0,0), typographic, &s1);
            g.MeasureString(p2, -1, &font40, PointF(0,0), typographic, &s2);

            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            SolidBrush redBrush(Color(255, 226, 35, 26));

            g.DrawString(p1, -1, &font40, PointF((REAL)margin, (REAL)y_pos + 15), typographic, &whiteBrush);
            g.DrawString(p2, -1, &font40, PointF((REAL)margin + s1.Width, (REAL)y_pos + 15), typographic, &redBrush);
            
            Font font13(memDC, pData->hFont13);
            SolidBrush textBrush(Color(255, 200, 200, 200));
            RectF rSub((REAL)margin, (REAL)y_pos + 15 + s1.Height + 10, (REAL)clientRect.right - margin, (REAL)clientRect.bottom - 80);
            g.DrawString(L"This quick tutorial will walk you through the key features and help with the initial setup.", -1, &font13, rSub, NULL, &textBrush);
        }
        else if (pData->page == 1)
        {
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            sfCenter.SetLineAlignment(StringAlignmentCenter);
            Font titleFont(memDC, pData->hFont20);
            Font font13(memDC, pData->hFont13);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            SolidBrush bodyBrush(Color(255, 210, 210, 210));

            g.DrawString(L"Start Here", -1, &titleFont, PointF(28.0f, 46.0f), &whiteBrush);
            RectF introRect(28.0f, 78.0f, (REAL)(clientRect.right - 56), 90.0f);
            g.DrawString(
                L"1. Open the main window from the tray icon near the clock.\n"
                L"2. Configurate settings in the General tab.\n"
                L"3. Press Start when Roblox is open.",
                -1, &font13, introRect, NULL, &bodyBrush);

            RectF rPrevLabel(0, 182.0f, (REAL)clientRect.right, 20.0f);
            g.DrawString(L"Tray icon status view:", -1, &font13, rPrevLabel, &sfCenter, &whiteBrush);

            int iconsY = 208;
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
        }
        else if (pData->page == 2)
        {
            Font titleFont(memDC, pData->hFont20);
            Font bodyFont(memDC, pData->hFont13);
            Font smallFont(memDC, pData->hFont11);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            SolidBrush bodyBrush(Color(255, 210, 210, 210));
            SolidBrush noteBrush(Color(255, 140, 140, 140));
            g.DrawString(L"Use Help Buttons", -1, &titleFont, PointF(28.0f, 46.0f), &whiteBrush);

            RectF introRect(28.0f, 78.0f, (REAL)(clientRect.right - 56), 70.0f);
            g.DrawString(L"When you see a small ? button next to a setting, click it. It explains what the option does and when you may want to use it.", -1, &bodyFont, introRect, NULL, &bodyBrush);

            const int demoTop = 172;
            const int demoRowH = 30;
            const int helpSize = 24;
            const int demoRight = clientRect.right - 28;
            RECT toggleDemoRect = { 28, demoTop, demoRight - helpSize, demoTop + demoRowH };
            MainUI_Paint_DrawToggle(memDC, toggleDemoRect, pData->hFont13, L"Auto-Start AntiAFK", true, false, 1.0f, true, L"\uE768");

            pData->demoHelpButtonRect = { demoRight - helpSize, demoTop + ((demoRowH - helpSize) / 2) + 4, demoRight, demoTop + ((demoRowH - helpSize) / 2) + helpSize + 4 };
            MainUI_Paint_DrawHelpButton(memDC, pData->demoHelpButtonRect, pData->hFont13, pData->isHoveringDemoHelp);

            RectF noteRect(28.0f, 220.0f, (REAL)(clientRect.right - 56), 22.0f);
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            g.DrawString(L"Try clicking the demo ? button.", -1, &smallFont, noteRect, &sfCenter, &noteBrush);
        }
        else if (pData->page == 3)
        {
            drawTextBlock(
                L"Simple Workflow",
                L"Use this simple routine:\n\n"
                L"Open Roblox.\n"
                L"Open AntiAFK-RBX.\n"
                L"Configurate settings in the General tab.\n"
                L"Press Start.\n\n"
                L"If something feels wrong, stop the app, change one option, and test again.");
        }
        else if (pData->page == 4)
        {
            Font titleFont(memDC, pData->hFont20);
            Font bodyFont(memDC, pData->hFont11);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            SolidBrush bodyBrush(Color(255, 210, 210, 210));
            g.DrawString(L"Recommended Settings", -1, &titleFont, PointF(28.0f, 46.0f), &whiteBrush);

            RectF bodyRect(28.0f, 78.0f, (REAL)(clientRect.right - 56), 164.0f);
            g.DrawString(
                L"Try these first:\n\n"
                L"\"Multi-Instance\" for multiple Roblox windows.\n"
                L"\"User-Safe Mode\" for safer behavior.\n"
                L"\"Auto-Start\" for launching with Windows.\n"
                L"\"Auto-Reconnect\" for reconnects after disconnects.\n"
                L"\"I can forget\" for full automation.\n"
                L"\"FPS Capper\" for reduce CPU and GPU usage.",
                -1, &bodyFont, bodyRect, NULL, &bodyBrush);
        }
        else if (pData->page == 5)
        {
            Font font20(memDC, pData->hFont20);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            
            StringFormat sfCenter;
            sfCenter.SetAlignment(StringAlignmentCenter);
            
            RectF rTop((REAL)0, (REAL)60, (REAL)clientRect.right, (REAL)30);
            g.DrawString(L"Thank for using", -1, &font20, rTop, &sfCenter, &whiteBrush);

            Font font40(memDC, pData->hFont40);
            const wchar_t* p1 = L"AntiAFK-";
            const wchar_t* p2 = L"RBX";
            
            RectF s1, s2;
            const StringFormat* typographic = StringFormat::GenericTypographic();
            g.MeasureString(p1, -1, &font40, PointF(0,0), typographic, &s1);
            g.MeasureString(p2, -1, &font40, PointF(0,0), typographic, &s2);
            
            REAL totalW = s1.Width + s2.Width;
            REAL startX = ((REAL)clientRect.right - totalW) / 2;
            
            SolidBrush redBrush(Color(255, 226, 35, 26));

            g.DrawString(p1, -1, &font40, PointF(startX, (REAL)rTop.GetBottom() - 5), typographic, &whiteBrush);
            g.DrawString(p2, -1, &font40, PointF(startX + s1.Width, (REAL)rTop.GetBottom() - 5), typographic, &redBrush);

            Font font13(memDC, pData->hFont13);
            SolidBrush grayBrush(Color(255, 200, 200, 200));
            RectF rBottom((REAL)20, (REAL)rTop.GetBottom() + s1.Height + 12, (REAL)(clientRect.right - 40), 48.0f);
            g.DrawString(L"You're all set! You can change settings later in the main window and reopen this tutorial from the tray menu.", -1, &font13, rBottom, &sfCenter, &grayBrush);

            pData->discordLinkRect = { 0, btnTop - 24, clientRect.right, btnTop };
            if (pData->isHoveringDiscordLink) {
                SolidBrush hoverBrush(Color(35, 55, 55, 55));
                g.FillRectangle(&hoverBrush, (REAL)pData->discordLinkRect.left, (REAL)pData->discordLinkRect.top, (REAL)(pData->discordLinkRect.right - pData->discordLinkRect.left), (REAL)(pData->discordLinkRect.bottom - pData->discordLinkRect.top));
            }
            Pen discordLinePen(Color(100, 80, 80, 80), 1.0f);
            g.DrawLine(&discordLinePen, (REAL)pData->discordLinkRect.left, (REAL)pData->discordLinkRect.top, (REAL)pData->discordLinkRect.right, (REAL)pData->discordLinkRect.top);
            Font linkFont(memDC, pData->hFont11);
            SolidBrush linkBrush(Color(255, 140, 170, 255));
            RectF linkRectF((REAL)pData->discordLinkRect.left, (REAL)pData->discordLinkRect.top, (REAL)(pData->discordLinkRect.right - pData->discordLinkRect.left), (REAL)(pData->discordLinkRect.bottom - pData->discordLinkRect.top));
            StringFormat linkCenter;
            linkCenter.SetAlignment(StringAlignmentCenter);
            linkCenter.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(L"Need help or have an idea? Join our Discord", -1, &linkFont, linkRectF, &linkCenter, &linkBrush);
        }

        auto drawBtn = [&](const RECT& r, const wchar_t* txt, bool hover, bool primary) {
            Popup_DrawActionButton(&g, memDC, r, pData->hFont13, txt, hover, primary);
        };

        auto drawLink = [&](const RECT& r, const wchar_t* txt, bool hover) {
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            
            Font font(memDC, pData->hFont11);
            SolidBrush brush(hover ? Color(255, 180, 180, 180) : Color(255, 119, 119, 119));
            
            StringFormat sf;
            if (pData->page == 0 || pData->page == 4 || pData->page == 2) {
                sf.SetAlignment(StringAlignmentCenter);
            } else {
                sf.SetAlignment(StringAlignmentNear);
            }
            sf.SetLineAlignment(StringAlignmentCenter);
            
            RectF rectF((REAL)r.left, (REAL)r.top, (REAL)(r.right - r.left), (REAL)(r.bottom - r.top));
            g.DrawString(txt, -1, &font, rectF, &sf, &brush);
        };

        drawBtn(pData->nextButtonRect, nextText, pData->isHoveringNext, true);
        if (pData->page > 0) drawBtn(pData->backButtonRect, L"Back", pData->isHoveringBack, false);
        if (pData->page == 0) drawBtn(pData->skipLinkRect, L"Skip", pData->isHoveringSkip, false);

        {
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            
            const int progressBarHeight = 4;
            SolidBrush bgBrush(Color(255, 45, 45, 45));
            g.FillRectangle(&bgBrush, (REAL)0, (REAL)(clientRect.bottom - progressBarHeight), (REAL)clientRect.right, (REAL)progressBarHeight);

            float targetProgress = (float)(pData->page + 1) / 6.0f;
            if (pData->progress < targetProgress) {
                pData->progress += 0.03f;
                if (pData->progress > targetProgress) pData->progress = targetProgress;
            }
            else if (pData->progress > targetProgress) {
                pData->progress -= 0.03f;
                if (pData->progress < targetProgress) pData->progress = targetProgress;
            }

            SolidBrush fgBrush(Color(255, 0, 122, 204));
            g.FillRectangle(&fgBrush, (REAL)0, (REAL)(clientRect.bottom - progressBarHeight), (REAL)(clientRect.right * pData->progress), (REAL)progressBarHeight);
        }

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
            if (pData->hFontTitle) DeleteObject(pData->hFontTitle);
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
        wc.hbrBackground = NULL;
        wc.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        registered = RegisterClass(&wc) != 0;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN), screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 380, winH = 520;
    int x = (screenW - winW) / 2, y = (screenH - winH) / 2 + 15;

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
struct NavItem {
    std::wstring title;
    std::wstring icon;
    RECT rect;
    float currentWidth;
};

struct MainUIData {
    HFONT hFontTitle = NULL, hFontSection = NULL, hFontText = NULL, hFontSmall = NULL, hFontNav = NULL, hFontNavSelected = NULL, hFontNavIcon = NULL;
    RECT closeButtonRect = { 0 };
    RECT iconButtonRect = { 0 };
    RECT startButtonRect = { 0 };

    int currentPage = 0; // 0: General, 1: Automation, 2: Stats, 3: Advanced, 4: Utils, 5: Discord
    std::vector<NavItem> navItems;
    int hoveringNavItem = -1;
    float navIndicatorX = 0.0f;
    float navIndicatorWidth = 0.0f;

    std::vector<RECT> rowRects;
    RECT intervalDropdownRect = { 0 };
    RECT actionDropdownRect = { 0 };
    RECT multiInstanceToggleRect = { 0 };
    RECT multiInstanceIntervalDropdownRect = { 0 };
    RECT fishstrapToggleRect = { 0 };
    RECT autoUpdateToggleRect = { 0 };
    RECT autoStartToggleRect = { 0 };
    RECT autoReconnectToggleRect = { 0 };
    RECT autoResetToggleRect = { 0 };
    RECT autoHideToggleRect = { 0 };
    RECT autoOpacityToggleRect = { 0 };
    RECT autoGridToggleRect = { 0 };
    RECT bloxstrapIntegrationToggleRect = { 0 };
    RECT userSafeDropdownRect = { 0 };
    RECT resetStatsButtonRect = { 0 };
    RECT statRowRects[4] = { 0 };
    RECT backupMessageRect = { 0 };
    RECT importSettingsRect = { 0 };
    RECT exportSettingsRect = { 0 };
    RECT unlockFpsOnFocusToggleRect = { 0 };
    RECT restoreMethodDropdownRect = { 0 };
    RECT legacyUiToggleRect = { 0 };
    RECT statusBarToggleRect = { 0 };
    RECT iCanForgetToggleRect = { 0 };
    RECT doNotSleepToggleRect = { 0 };
    RECT autoMuteToggleRect = { 0 };
    RECT unmuteOnFocusToggleRect = { 0 };
    std::vector<RECT> helpButtonRects;
    RECT fpsCapperDropdownRect = { 0 };
    RECT resetSettingsButtonRect = { 0 };
    RECT updateBannerRect = { 0 };
    RECT discordButtonRect = { 0 };
    RECT discordWebhookEnableToggleRect = { 0 };
    RECT discordWebhookInputRect = { 0 };
    RECT discordWebhookPasteRect = { 0 };
    RECT discordWebhookClearRect = { 0 };
    RECT discordWebhookTestRect = { 0 };
    RECT discordNotifyStartToggleRect = { 0 };
    RECT discordNotifyStopToggleRect = { 0 };
    RECT discordNotifyActionToggleRect = { 0 };
    RECT discordNotifyReconnectToggleRect = { 0 };
    RECT discordNotifyResetToggleRect = { 0 };
    RECT discordNotifyErrorsToggleRect = { 0 };
    RECT discordDisableEmbedToggleRect = { 0 };
    RECT discordMentionOnErrorsToggleRect = { 0 };
    RECT discordPayloadInfoRect = { 0 };
    RECT disclaimerRect = { 0 };
    RECT utilsShowRobloxRect = { 0 };
    RECT utilsHideRobloxRect = { 0 };
    RECT utilsGridSnapRect = { 0 };
    RECT utilsWindowOpacityRect = { 0 };
    RECT utilsCloseAllRect = { 0 };
    RECT utilsResetAllRect = { 0 };
    RECT utilsTestActionRect = { 0 };
    RECT utilsMuteToggleRect = { 0 };
    RECT utilsFpsToggleRect = { 0 };

    POINT hoverPoint = { 0 };
    bool isHoveringClose = false;
    bool isHoveringIcon = false;
    bool isHoveringStart = false;
    float startButtonScale = 1.0f;
    float startButtonVelocity = 0.0f;
    bool isPressingStart = false;
    bool isHoveringDiscord = false;
    bool isHoveringUpdateBanner = false;
    bool isHoveringReset = false;
    bool isHoveringImportSettings = false;
    bool isHoveringExportSettings = false;
    bool isHoveringResetStats = false;
    bool isHoveringDisclaimer = false;
    bool isHoveringDiscordWebhookInput = false;
    bool isHoveringDiscordWebhookPaste = false;
    bool isHoveringDiscordWebhookClear = false;
    bool isHoveringDiscordWebhookTest = false;
    bool isHoveringUtilsShowRoblox = false;
    bool isHoveringUtilsHideRoblox = false;
    bool isHoveringUtilsGridSnap = false;
    bool isHoveringUtilsWindowOpacity = false;
    bool isHoveringUtilsCloseAll = false;
    bool isHoveringUtilsResetAll = false;
    bool isHoveringUtilsTestAction = false;
    bool isHoveringUtilsMuteToggle = false;
    bool isHoveringUtilsFpsToggle = false;
    bool isDiscordWebhookInputFocused = false;
    size_t discordWebhookCaretPos = 0;
    size_t discordWebhookViewOffset = 0;
    bool isHoveringInterval = false;
    bool isHoveringAction = false;
    bool isHoveringUserSafe = false;
    bool isHoveringRestore = false;
    bool isHoveringFpsCapper = false;
    bool isHoveringMultiInstanceInterval = false;
    int hoveringHelpButton = -1;

    std::wstring startButtonErrorText;
    uint64_t startButtonErrorTime = 0;
    float errorAnimationProgress = 0.0f; 
    int errorAnimationDirection = 0;

    UINT_PTR uTimerId = 0;
    bool isTrackingMouse = false;
    HCURSOR hCursorHand = NULL, hCursorArrow = NULL, hCursorText = NULL;
    HICON hIcon = NULL;

    float multiInstanceAnim = 0.0f;
    float fishstrapAnim = 0.0f;
    float autoUpdateAnim = 0.0f;
    float autoStartAnim = 0.0f;
    float autoReconnectAnim = 0.0f;
    float autoResetAnim = 0.0f;
    float autoHideAnim = 0.0f;
    float autoOpacityAnim = 0.0f;
    float autoGridAnim = 0.0f;
    float bloxstrapIntegrationAnim = 0.0f;
    float unlockFpsOnFocusAnim = 0.0f;
    float legacyUiAnim = 0.0f;
    float statusBarAnim = 0.0f;
    float iCanForgetAnim = 0.0f;
    float doNotSleepAnim = 0.0f;
    float autoMuteAnim = 0.0f;
    float unmuteOnFocusAnim = 0.0f;
    float utilsWindowOpacityAnim = 0.0f;
    float utilsMuteAnim = 0.0f;
    float utilsFpsAnim = 0.0f;
    float discordWebhookEnableAnim = 0.0f;
    float discordNotifyStartAnim = 0.0f;
    float discordNotifyStopAnim = 0.0f;
    float discordNotifyActionAnim = 0.0f;
    float discordNotifyReconnectAnim = 0.0f;
    float discordNotifyResetAnim = 0.0f;
    float discordNotifyErrorsAnim = 0.0f;
    float discordDisableEmbedAnim = 0.0f;
    float discordMentionOnErrorsAnim = 0.0f;

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

struct StatusBarData {
    HFONT hFontBrand = NULL;
    HFONT hFontText = NULL;
    HICON hIcon = NULL;
    std::wstring message = L"Ready";
    RECT targetBounds = { 0 };
    BYTE currentAlpha = 0;
    BYTE targetAlpha = 0;
};

LRESULT CALLBACK StatusBarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int MeasureTextWidth(HDC hdc, HFONT font, const std::wstring& text)
{
    if (!hdc || !font || text.empty()) {
        return 0;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SIZE size = { 0 };
    GetTextExtentPoint32W(hdc, text.c_str(), (int)text.size(), &size);
    SelectObject(hdc, oldFont);
    return size.cx;
}

int MeasureStatusBarContentWidth(const std::wstring& message, int dpiY)
{
    HDC hdc = GetDC(NULL);
    if (!hdc) {
        return 360;
    }

    HFONT brandFont = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT textFont = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    int brandWidth = MeasureTextWidth(hdc, brandFont, L"AntiAFK-RBX");
    int separatorWidth = MeasureTextWidth(hdc, textFont, L" \u2022 ");
    int messageWidth = MeasureTextWidth(hdc, textFont, message);

    if (brandFont) DeleteObject(brandFont);
    if (textFont) DeleteObject(textFont);
    ReleaseDC(NULL, hdc);

    return MulDiv(10, dpiY, 96) + MulDiv(14, dpiY, 96) + MulDiv(10, dpiY, 96)
        + brandWidth + separatorWidth + messageWidth + MulDiv(26, dpiY, 96);
}

HMONITOR ResolveStatusBarMonitor(HWND anchorWindow)
{
    if (anchorWindow && IsWindow(anchorWindow)) {
        return MonitorFromWindow(anchorWindow, MONITOR_DEFAULTTONEAREST);
    }

    POINT cursorPt = { 0 };
    if (GetCursorPos(&cursorPt)) {
        return MonitorFromPoint(cursorPt, MONITOR_DEFAULTTONEAREST);
    }

    HWND fg = GetForegroundWindow();
    if (fg && IsWindow(fg)) {
        return MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    }

    if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
        return MonitorFromWindow(g_hMainUiWnd, MONITOR_DEFAULTTONEAREST);
    }

    if (g_hwnd && IsWindow(g_hwnd)) {
        return MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTONEAREST);
    }

    return MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
}

RECT CalculateStatusBarBounds(HWND anchorWindow, const std::wstring& message)
{
    HDC screen = GetDC(NULL);
    int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
    int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
    ReleaseDC(NULL, screen);

    MONITORINFO mi = { sizeof(mi) };
    HMONITOR hMon = ResolveStatusBarMonitor(anchorWindow);
    GetMonitorInfo(hMon, &mi);

    int workWidth = mi.rcWork.right - mi.rcWork.left;
    int minWidth = MulDiv(332, dpiX, 96);
    int width = (std::max)(minWidth, MeasureStatusBarContentWidth(message, dpiY));
    int height = MulDiv(34, dpiY, 96);
    width = (std::min)(width, workWidth - MulDiv(54, dpiX, 96));
    width = (std::max)(width, minWidth);

    int x = mi.rcWork.left + (workWidth - width) / 2;
    int y = mi.rcWork.top + MulDiv(14, dpiY, 96);
    return { x, y, x + width, y + height };
}

void UpdateStatusBarWindowRegion(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    HDC wndDC = GetDC(hwnd);
    int dpiY = wndDC ? GetDeviceCaps(wndDC, LOGPIXELSY) : 96;
    if (wndDC) ReleaseDC(hwnd, wndDC);
    int radius = (std::min)(MulDiv(10, dpiY, 96), height / 2);
    HRGN region = CreateRoundRectRgn(0, 0, width, height, radius * 2, radius * 2);
    SetWindowRgn(hwnd, region, TRUE);
}

void UpdateStatusBarPlacement(HWND hwnd, StatusBarData* pData)
{
    if (!pData) {
        return;
    }

    int width = pData->targetBounds.right - pData->targetBounds.left;
    int height = pData->targetBounds.bottom - pData->targetBounds.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    SetWindowPos(hwnd, HWND_TOPMOST,
        pData->targetBounds.left, pData->targetBounds.top,
        width, height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    UpdateStatusBarWindowRegion(hwnd);
}

void HideStatusBarOverlay(bool animate = false)
{
    if (!g_hStatusBarWnd || !IsWindow(g_hStatusBarWnd)) {
        return;
    }

    KillTimer(g_hStatusBarWnd, STATUS_BAR_HIDE_TIMER);
    StatusBarData* pData = (StatusBarData*)GetWindowLongPtr(g_hStatusBarWnd, GWLP_USERDATA);
    if (!pData || !animate || !IsWindowVisible(g_hStatusBarWnd)) {
        KillTimer(g_hStatusBarWnd, STATUS_BAR_ANIM_TIMER);
        if (pData) {
            pData->currentAlpha = 0;
            pData->targetAlpha = 0;
        }
        SetLayeredWindowAttributes(g_hStatusBarWnd, 0, 0, LWA_ALPHA);
        ShowWindow(g_hStatusBarWnd, SW_HIDE);
        return;
    }
    pData->targetAlpha = 0;
    SetTimer(g_hStatusBarWnd, STATUS_BAR_ANIM_TIMER, 16, NULL);
}

void EnsureStatusBarWindow()
{
    if (g_hStatusBarWnd && IsWindow(g_hStatusBarWnd)) {
        return;
    }

    const wchar_t STATUS_BAR_CLASS_NAME[] = L"AntiAFK-RBX-StatusBar";
    static bool registered = false;

    if (!registered) {
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = StatusBarWndProc;
        wc.hInstance = g_hInst;
        wc.lpszClassName = STATUS_BAR_CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return;
        }
        registered = true;
    }

    g_hStatusBarWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        STATUS_BAR_CLASS_NAME,
        L"",
        WS_POPUP,
        0, 0, 360, 40,
        NULL, NULL, g_hInst, NULL);
}

void QueueStatusBarOverlay(const std::wstring& message, UINT durationMs, HWND anchorWindow)
{
    if (!g_statusBarEnabled.load()) {
        return;
    }

    if (!g_hwnd || !IsWindow(g_hwnd)) {
        return;
    }

    StatusBarPayload* payload = new StatusBarPayload{ message, durationMs, anchorWindow };
    if (!PostMessage(g_hwnd, WM_APP_SHOW_STATUS_BAR, reinterpret_cast<WPARAM>(payload), 0)) {
        delete payload;
    }
}

void ShowStatusBarOverlay(const std::wstring& message, UINT durationMs, HWND anchorWindow)
{
    if (!g_statusBarEnabled.load()) {
        HideStatusBarOverlay(false);
        return;
    }

    EnsureStatusBarWindow();
    if (!g_hStatusBarWnd || !IsWindow(g_hStatusBarWnd)) {
        return;
    }

    StatusBarData* pData = (StatusBarData*)GetWindowLongPtr(g_hStatusBarWnd, GWLP_USERDATA);
    if (!pData) {
        return;
    }

    pData->message = message;
    if (pData->hIcon) {
        DestroyIcon(pData->hIcon);
        pData->hIcon = NULL;
    }
    pData->hIcon = CreateCustomIcon();
    pData->targetBounds = CalculateStatusBarBounds(anchorWindow, message);
    bool wasVisible = IsWindowVisible(g_hStatusBarWnd) != FALSE;
    UpdateStatusBarPlacement(g_hStatusBarWnd, pData);
    KillTimer(g_hStatusBarWnd, STATUS_BAR_HIDE_TIMER);
    SetTimer(g_hStatusBarWnd, STATUS_BAR_HIDE_TIMER, (std::max)(durationMs, (UINT)700), NULL);

    pData->targetAlpha = 255;
    if (!wasVisible) {
        pData->currentAlpha = 0;
        SetLayeredWindowAttributes(g_hStatusBarWnd, 0, 0, LWA_ALPHA);
        ShowWindow(g_hStatusBarWnd, SW_SHOWNOACTIVATE);
    }
    SetTimer(g_hStatusBarWnd, STATUS_BAR_ANIM_TIMER, 16, NULL);
    InvalidateRect(g_hStatusBarWnd, NULL, FALSE);
    UpdateWindow(g_hStatusBarWnd);
}

void StatusBar_Paint(HDC hdc, const RECT& clientRect, StatusBarData* pData)
{
    if (!pData) {
        return;
    }

    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetPixelOffsetMode(PixelOffsetModeDefault);

    const REAL width = (REAL)(clientRect.right - clientRect.left);
    const REAL height = (REAL)(clientRect.bottom - clientRect.top);
    const REAL radius = 9.0f;
    const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    const int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    const int iconLeftPx = MulDiv(10, dpiX, 96);
    const int iconSizePx = MulDiv(14, dpiY, 96);
    const int iconGapPx = MulDiv(10, dpiX, 96);
    const int horizontalPaddingPx = MulDiv(12, dpiX, 96);

    SolidBrush bodyBrush(Color(90, 16, 16, 16));
    Pen borderPen(Color(58, 255, 255, 255), 1.0f);

    FillRoundedRectangle(&g, &bodyBrush, 0.0f, 0.0f, width, height, radius);
    DrawRoundedRectangle(&g, &borderPen, 0.0f, 0.0f, width - 1.0f, height - 1.0f, radius);

    Font brandFont(hdc, pData->hFontBrand);
    Font textFont(hdc, pData->hFontText);
    SolidBrush brandBrush(Color(255, 255, 255, 255));
    SolidBrush textBrush(Color(255, 220, 220, 220));

    const wchar_t* brand = L"AntiAFK-RBX";
    const wchar_t* separator = L" \u2022 ";

    REAL y = 0.0f;

    StringFormat sfFixed;
    sfFixed.SetAlignment(StringAlignmentNear);
    sfFixed.SetLineAlignment(StringAlignmentCenter);
    sfFixed.SetTrimming(StringTrimmingNone);
    sfFixed.SetFormatFlags(StringFormatFlagsNoWrap);

    StringFormat sfStatus;
    sfStatus.SetAlignment(StringAlignmentNear);
    sfStatus.SetLineAlignment(StringAlignmentCenter);
    sfStatus.SetTrimming(StringTrimmingEllipsisCharacter);
    sfStatus.SetFormatFlags(StringFormatFlagsNoWrap);

    int brandWidthPx = MeasureTextWidth(hdc, pData->hFontBrand, brand);
    int separatorWidthPx = MeasureTextWidth(hdc, pData->hFontText, separator);
    int messageWidthPx = MeasureTextWidth(hdc, pData->hFontText, pData->message);

    const REAL fixedTextLeft = (REAL)(iconLeftPx + iconSizePx + iconGapPx);
    REAL availableTextWidth = (std::max)(0.0f, width - fixedTextLeft - (REAL)horizontalPaddingPx);
    REAL textBlockWidth = (REAL)brandWidthPx + (REAL)separatorWidthPx + (REAL)messageWidthPx;
    REAL x = fixedTextLeft;
    if (textBlockWidth < availableTextWidth) {
        x += (availableTextWidth - textBlockWidth) / 2.0f;
    }

    if (pData->hIcon) {
        DrawIconEx(hdc, iconLeftPx, (int)((height - (REAL)iconSizePx) / 2.0f), pData->hIcon, iconSizePx, iconSizePx, 0, NULL, DI_NORMAL);
    }

    RectF brandRect(x, y, (REAL)brandWidthPx + 8.0f, height);
    g.DrawString(brand, -1, &brandFont, brandRect, &sfFixed, &brandBrush);

    x += (REAL)brandWidthPx;

    RectF separatorRect(x, y, (REAL)separatorWidthPx + 4.0f, height);
    g.DrawString(separator, -1, &textFont, separatorRect, &sfFixed, &textBrush);

    x += (REAL)separatorWidthPx;

    RectF statusRect(x, y, width - x - (REAL)horizontalPaddingPx, height);
    g.DrawString(pData->message.c_str(), -1, &textFont, statusRect, &sfStatus, &textBrush);
}

LRESULT CALLBACK StatusBarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    StatusBarData* pData = (StatusBarData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        pData = new StatusBarData();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);

        HDC screen = GetDC(NULL);
        int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        pData->hFontBrand = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hFontText = CreateFontW(-MulDiv(9, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        pData->hIcon = CreateCustomIcon();
        pData->currentAlpha = 0;
        pData->targetAlpha = 0;

        EnableAcrylic(hwnd);
        UpdateStatusBarWindowRegion(hwnd);
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);

        enum DWM_WINDOW_CORNER_PREFERENCE {
            DWMWCP_DEFAULT = 0,
            DWMWCP_DONOTROUND = 1,
            DWMWCP_ROUND = 2,
            DWMWCP_ROUNDSMALL = 3
        };
        const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        UpdateStatusBarWindowRegion(hwnd);
        return 0;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_TIMER:
        if (!pData) {
            return 0;
        }

        if (wParam == STATUS_BAR_HIDE_TIMER) {
            KillTimer(hwnd, STATUS_BAR_HIDE_TIMER);
            HideStatusBarOverlay(true);
            return 0;
        }
        if (wParam == STATUS_BAR_ANIM_TIMER) {
            int currentAlpha = (int)pData->currentAlpha;
            int targetAlpha = (int)pData->targetAlpha;

            if (currentAlpha < targetAlpha) {
                currentAlpha = (std::min)(targetAlpha, currentAlpha + 28);
            }
            else if (currentAlpha > targetAlpha) {
                currentAlpha = (std::max)(targetAlpha, currentAlpha - 24);
            }

            pData->currentAlpha = (BYTE)currentAlpha;
            SetLayeredWindowAttributes(hwnd, 0, pData->currentAlpha, LWA_ALPHA);

            if (pData->currentAlpha == pData->targetAlpha) {
                KillTimer(hwnd, STATUS_BAR_ANIM_TIMER);
                if (pData->currentAlpha == 0) {
                    ShowWindow(hwnd, SW_HIDE);
                }
            }
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

        HBRUSH backBrush = CreateSolidBrush(RGB(16, 16, 16));
        FillRect(memDC, &clientRect, backBrush);
        DeleteObject(backBrush);
        StatusBar_Paint(memDC, clientRect, pData);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBMP);
        DeleteObject(memBMP);
        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        if (pData) {
            KillTimer(hwnd, STATUS_BAR_HIDE_TIMER);
            KillTimer(hwnd, STATUS_BAR_ANIM_TIMER);
            if (pData->hIcon) DestroyIcon(pData->hIcon);
            if (pData->hFontBrand) DeleteObject(pData->hFontBrand);
            if (pData->hFontText) DeleteObject(pData->hFontText);
            delete pData;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        g_hStatusBarWnd = NULL;
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void MainUI_Paint_DrawCloseButton(HDC hdc, const RECT& closeButtonRect, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    int x_center = (closeButtonRect.left + closeButtonRect.right) / 2;
    int y_center = (closeButtonRect.top + closeButtonRect.bottom) / 2;
    int cross_size = 5;

    DrawSharedCloseButton(&g, closeButtonRect, isHovering);
}
void MainUI_Paint_DrawToggle(HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool checked, bool isHovering, float animState, bool hasRightNeighbor, const wchar_t* icon = nullptr) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    int iconOffset = 0;
    if (icon) {
        HFONT iconFontH = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font iconFont(hdc, iconFontH);
        SolidBrush iconBrush(Color(255, 150, 150, 150));
        int iconSize = 16;
        int iconPadding = 3;
        int iconY = rect.top + (rect.bottom - rect.top - iconSize) / 2 + 5 - iconPadding;
        RectF iconRectF((REAL)16, (REAL)iconY, (REAL)(iconSize + iconPadding * 2), (REAL)(iconSize + iconPadding * 2));
        StringFormat sfIcon;
        sfIcon.SetAlignment(StringAlignmentCenter);
        sfIcon.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(icon, -1, &iconFont, iconRectF, &sfIcon, &iconBrush);
        DeleteObject(iconFontH);
        iconOffset = iconSize + 10;
    }

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sfLeft;
    sfLeft.SetAlignment(StringAlignmentNear);
    sfLeft.SetLineAlignment(StringAlignmentCenter);
    int labelWidth = rect.right - rect.left - 66 - iconOffset;
    int labelMargin = 16 + iconOffset;
    RectF labelRectF((REAL)labelMargin, (REAL)rect.top + 4, (REAL)labelWidth, (REAL)(rect.bottom - rect.top));
    g.DrawString(text, -1, &gdiFont, labelRectF, &sfLeft, &textBrush);

    int toggleW = 50, toggleH = 24;
    int toggleX = rect.right - toggleW;
    int toggleY = rect.top + (rect.bottom - rect.top - toggleH) / 2 + 4;
    RECT toggleBackRect = { toggleX, toggleY, toggleX + toggleW, toggleY + toggleH };

    SolidBrush backBrush(isHovering ? Color(140, 65, 65, 65) : Color(140, 45, 45, 45));
    Pen borderPen(checked ? Color(255, 0, 122, 204) : Color(180, 80, 80, 80), checked ? 1.5f : 1.0f);

    GraphicsPath path;
    REAL r = 6.0f;
    path.AddArc((REAL)toggleBackRect.left, (REAL)toggleBackRect.top, r * 2, r * 2, 180, 90);
    path.AddLine((REAL)(toggleBackRect.left + r), (REAL)toggleBackRect.top, (REAL)toggleBackRect.right, (REAL)toggleBackRect.top);
    path.AddLine((REAL)toggleBackRect.right, (REAL)toggleBackRect.top, (REAL)toggleBackRect.right, (REAL)toggleBackRect.bottom);
    path.AddLine((REAL)toggleBackRect.right, (REAL)toggleBackRect.bottom, (REAL)(toggleBackRect.left + r), (REAL)toggleBackRect.bottom);
    path.AddArc((REAL)toggleBackRect.left, (REAL)(toggleBackRect.bottom - r * 2), r * 2, r * 2, 90, 90);
    path.CloseFigure();
    g.FillPath(&backBrush, &path);
    g.DrawPath(&borderPen, &path);

    SolidBrush knobBrush(checked ? Color(255, 0, 122, 204) : Color(255, 140, 140, 140));
    int knobSize = toggleH - 8;
    int startX = toggleBackRect.left + 4;
    int endX = toggleBackRect.right - knobSize - 4;
    int knobX = startX + (int)((endX - startX) * animState);
    int knobY = toggleBackRect.top + 4;
    g.FillEllipse(&knobBrush, knobX, knobY, knobSize, knobSize);
}
void MainUI_Paint_DrawDropdown(HDC hdc, const RECT& rect, HFONT font, const wchar_t* label, const wchar_t* value, bool isHovering, bool hasRightNeighbor, const wchar_t* icon = nullptr) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sfLeft;
    sfLeft.SetAlignment(StringAlignmentNear);
    sfLeft.SetLineAlignment(StringAlignmentCenter);

    int iconOffset = 0;
    if (icon) {
        HFONT iconFontH = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font iconFont(hdc, iconFontH);
        SolidBrush iconBrush(Color(255, 150, 150, 150));
        int iconSize = 16;
        int iconPadding = 3;
        int iconY = rect.top + (rect.bottom - rect.top - iconSize) / 2 + 1 - iconPadding;
        RectF iconRectF((REAL)16, (REAL)iconY, (REAL)(iconSize + iconPadding * 2), (REAL)(iconSize + iconPadding * 2));
        StringFormat sfIcon;
        sfIcon.SetAlignment(StringAlignmentCenter);
        sfIcon.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(icon, -1, &iconFont, iconRectF, &sfIcon, &iconBrush);
        DeleteObject(iconFontH);
        iconOffset = iconSize + 10;
    }

    int labelWidth = 150;
    RectF labelRectF((REAL)(16 + iconOffset), (REAL)rect.top, (REAL)labelWidth, (REAL)(rect.bottom - rect.top));
    g.DrawString(label, -1, &gdiFont, labelRectF, &sfLeft, &textBrush);

    SolidBrush backBrush(isHovering ? Color(140, 65, 65, 65) : Color(140, 45, 45, 45));
    Pen borderPen(Color(180, 80, 80, 80));

    if (hasRightNeighbor) {
        GraphicsPath path;
        REAL r = 5.0f;
        path.AddArc((REAL)rect.left, (REAL)rect.top, r * 2, r * 2, 180, 90);
        path.AddLine((REAL)(rect.left + r), (REAL)rect.top, (REAL)rect.right, (REAL)rect.top);
        path.AddLine((REAL)rect.right, (REAL)rect.top, (REAL)rect.right, (REAL)rect.bottom);
        path.AddLine((REAL)rect.right, (REAL)rect.bottom, (REAL)(rect.left + r), (REAL)rect.bottom);
        path.AddArc((REAL)rect.left, (REAL)(rect.bottom - r * 2), r * 2, r * 2, 90, 90);
        path.CloseFigure();
        g.FillPath(&backBrush, &path);
        g.DrawPath(&borderPen, &path);
    } else {
        FillRoundedRectangle(&g, &backBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top), 5);
        DrawRoundedRectangle(&g, &borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top), 5);
    }

    RECT valueRect = rect;
    InflateRect(&valueRect, -8, 0);
    RectF valueRectF((REAL)valueRect.left, (REAL)valueRect.top, (REAL)(valueRect.right - valueRect.left), (REAL)(valueRect.bottom - valueRect.top));
    g.DrawString(value, -1, &gdiFont, valueRectF, &sfLeft, &textBrush);

    Pen arrowPen(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)), 2.0f);
    int arrowX = rect.right - 15;
    int arrowY = rect.top + (rect.bottom - rect.top) / 2;
    g.DrawLine(&arrowPen, (REAL)(arrowX - 4), (REAL)(arrowY - 2), (REAL)arrowX, (REAL)(arrowY + 2));
    g.DrawLine(&arrowPen, (REAL)arrowX, (REAL)(arrowY + 2), (REAL)(arrowX + 4), (REAL)(arrowY - 2));

    if (hasRightNeighbor) {
        Pen sepPen(Color(180, 80, 80, 80), 1.0f);
        g.DrawLine(&sepPen, (REAL)rect.right, (REAL)(rect.top + 4), (REAL)rect.right, (REAL)(rect.bottom - 4));
    }
}
void MainUI_Paint_DrawActionButton(HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool isHovering, bool isPrimary) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Color fillColor = isPrimary
        ? (isHovering ? Color(170, 20, 142, 224) : Color(140, 0, 122, 204))
        : (isHovering ? Color(70, 60, 60, 60) : Color(50, 50, 50, 50));
    Color borderColor = isPrimary ? Color(180, 0, 122, 204) : Color(80, 70, 70, 70);

    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);
    g.FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawRectangle(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left - 1), (REAL)(rect.bottom - rect.top - 1));

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF rectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawString(text, -1, &textFont, rectF, &sf, &textBrush);
}
void MainUI_Paint_DrawListActionRow(HDC hdc, const RECT& rect, HFONT font, const wchar_t* text, bool isHovering, const wchar_t* icon = nullptr)
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    if (isHovering) {
        SolidBrush hoverBrush(Color(45, 60, 60, 60));
        g.FillRectangle(&hoverBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    }

    HFONT iconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
    HFONT overlayIconFontH = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
    Font iconFont(hdc, iconFontH);
    Font overlayIconFont(hdc, overlayIconFontH);
    Font textFont(hdc, font);

    SolidBrush iconBrush(Color(255, 150, 150, 150));
    SolidBrush textBrush(Color(255, 220, 220, 220));
    SolidBrush arrowBrush(Color(255, 150, 150, 150));

    int contentLeft = rect.left + 20;
    if (icon && icon[0] != L'\0') {
        RectF iconRect((REAL)contentLeft, (REAL)rect.top, 16.0f, (REAL)(rect.bottom - rect.top));
        StringFormat sfIcon;
        sfIcon.SetAlignment(StringAlignmentCenter);
        sfIcon.SetLineAlignment(StringAlignmentCenter);
        size_t iconLen = wcslen(icon);
        if (iconLen >= 2) {
            wchar_t baseGlyph[2] = { icon[0], L'\0' };
            wchar_t overlayGlyph[2] = { icon[1], L'\0' };
            SolidBrush overlayBrush(Color(255, 150, 150, 150));
            g.DrawString(baseGlyph, -1, &iconFont, iconRect, &sfIcon, &iconBrush);
            PointF overlayPos((REAL)contentLeft - 2.0f, (REAL)rect.top + 14.0f);
            g.DrawString(overlayGlyph, -1, &overlayIconFont, overlayPos, &overlayBrush);
        } else {
            g.DrawString(icon, -1, &iconFont, iconRect, &sfIcon, &iconBrush);
        }
        contentLeft += 24;
    }

    RectF textRect((REAL)contentLeft, (REAL)rect.top, (REAL)(rect.right - contentLeft - 40), (REAL)(rect.bottom - rect.top));
    StringFormat sfText;
    sfText.SetAlignment(StringAlignmentNear);
    sfText.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(text, -1, &textFont, textRect, &sfText, &textBrush);

    RectF arrowRect((REAL)(rect.right - 24), (REAL)rect.top, 12.0f, (REAL)(rect.bottom - rect.top));
    StringFormat sfArrow;
    sfArrow.SetAlignment(StringAlignmentCenter);
    sfArrow.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(L"\uE76C", -1, &iconFont, arrowRect, &sfArrow, &arrowBrush);

    DeleteObject(iconFontH);
    DeleteObject(overlayIconFontH);
}

void MainUI_Paint_DrawBackupCard(HDC hdc, const RECT& cardRect, HFONT font, bool importHover, bool exportHover)
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush cardBrush(Color(90, 35, 35, 35));
    Pen cardPen(Color(90, 80, 80, 80), 1.0f);
    FillRoundedRectangle(&g, &cardBrush, (REAL)cardRect.left, (REAL)cardRect.top, (REAL)(cardRect.right - cardRect.left), (REAL)(cardRect.bottom - cardRect.top), 8);
    DrawRoundedRectangle(&g, &cardPen, (REAL)cardRect.left, (REAL)cardRect.top, (REAL)(cardRect.right - cardRect.left), (REAL)(cardRect.bottom - cardRect.top), 8);

    Font titleFont(hdc, font);
    SolidBrush titleBrush(Color(255, 220, 220, 220));
    StringFormat titleFmt;
    titleFmt.SetAlignment(StringAlignmentCenter);
    titleFmt.SetLineAlignment(StringAlignmentCenter);
    RECT titleRect = { cardRect.left, cardRect.top + 2, cardRect.right, cardRect.top + 18 };
    RectF titleRectF((REAL)titleRect.left, (REAL)titleRect.top, (REAL)(titleRect.right - titleRect.left), (REAL)(titleRect.bottom - titleRect.top));
    g.DrawString(L"settings backups", -1, &titleFont, titleRectF, &titleFmt, &titleBrush);

    int innerPadding = 6;
    int gap = 1;
    RECT importRect = { cardRect.left + innerPadding, cardRect.top + 20, cardRect.left + (cardRect.right - cardRect.left - innerPadding - gap) / 2, cardRect.bottom - innerPadding };
    RECT exportRect = { importRect.right + gap, cardRect.top + 20, cardRect.right - innerPadding, cardRect.bottom - innerPadding };

    MainUI_Paint_DrawActionButton(hdc, importRect, font, L"Import", importHover, false);
    MainUI_Paint_DrawActionButton(hdc, exportRect, font, L"Export", exportHover, true);
}

void MainUI_Paint_DrawBackupButtonsRow(HDC hdc, const RECT& importRect, const RECT& exportRect, HFONT font, bool importHover, bool exportHover)
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RECT groupRect = { importRect.left, importRect.top, exportRect.right, importRect.bottom };
    Color groupBorderColor(92, 72, 72, 72);
    Color importFill = importHover ? Color(108, 58, 58, 58) : Color(84, 42, 42, 42);
    Color exportFill = exportHover ? Color(108, 58, 58, 58) : Color(84, 42, 42, 42);

    SolidBrush importBrush(importFill);
    SolidBrush exportBrush(exportFill);
    Pen borderPen(groupBorderColor, 1.0f);

    g.FillRectangle(&importBrush, (REAL)importRect.left, (REAL)importRect.top, (REAL)(importRect.right - importRect.left), (REAL)(importRect.bottom - importRect.top));
    g.FillRectangle(&exportBrush, (REAL)exportRect.left, (REAL)exportRect.top, (REAL)(exportRect.right - exportRect.left), (REAL)(exportRect.bottom - exportRect.top));

    g.DrawRectangle(&borderPen, (REAL)groupRect.left, (REAL)groupRect.top, (REAL)(groupRect.right - groupRect.left - 1), (REAL)(groupRect.bottom - groupRect.top - 1));
    g.DrawLine(&borderPen, (REAL)exportRect.left, (REAL)(groupRect.top + 1), (REAL)exportRect.left, (REAL)(groupRect.bottom - 2));

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    RectF importRectF((REAL)importRect.left, (REAL)importRect.top, (REAL)(importRect.right - importRect.left), (REAL)(importRect.bottom - importRect.top));
    RectF exportRectF((REAL)exportRect.left, (REAL)exportRect.top, (REAL)(exportRect.right - exportRect.left), (REAL)(exportRect.bottom - exportRect.top));
    g.DrawString(L"Import", -1, &textFont, importRectF, &sf, &textBrush);
    g.DrawString(L"Export", -1, &textFont, exportRectF, &sf, &textBrush);
}
void MainUI_Paint_DrawTripleActionButtonsRow(HDC hdc, const RECT& leftRect, const RECT& middleRect, const RECT& rightRect, HFONT font,
    const wchar_t* leftText, const wchar_t* middleText, const wchar_t* rightText,
    bool leftHover, bool middleHover, bool rightHover, bool rightPrimary)
{
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RECT groupRect = { leftRect.left, leftRect.top, rightRect.right, leftRect.bottom };
    Color groupBorderColor(92, 72, 72, 72);
    Color leftFill = leftHover ? Color(108, 58, 58, 58) : Color(84, 42, 42, 42);
    Color middleFill = middleHover ? Color(108, 58, 58, 58) : Color(84, 42, 42, 42);
    Color rightFill = rightPrimary
        ? (rightHover ? Color(140, 20, 142, 224) : Color(110, 0, 122, 204))
        : (rightHover ? Color(108, 58, 58, 58) : Color(84, 42, 42, 42));

    SolidBrush leftBrush(leftFill);
    SolidBrush middleBrush(middleFill);
    SolidBrush rightBrush(rightFill);
    Pen borderPen(groupBorderColor, 1.0f);
    Pen primaryDividerPen(rightPrimary ? Color(150, 0, 122, 204) : groupBorderColor, 1.0f);

    g.FillRectangle(&leftBrush, (REAL)leftRect.left, (REAL)leftRect.top, (REAL)(leftRect.right - leftRect.left), (REAL)(leftRect.bottom - leftRect.top));
    g.FillRectangle(&middleBrush, (REAL)middleRect.left, (REAL)middleRect.top, (REAL)(middleRect.right - middleRect.left), (REAL)(middleRect.bottom - middleRect.top));
    g.FillRectangle(&rightBrush, (REAL)rightRect.left, (REAL)rightRect.top, (REAL)(rightRect.right - rightRect.left), (REAL)(rightRect.bottom - rightRect.top));

    g.DrawRectangle(&borderPen, (REAL)groupRect.left, (REAL)groupRect.top, (REAL)(groupRect.right - groupRect.left - 1), (REAL)(groupRect.bottom - groupRect.top - 1));
    g.DrawLine(&borderPen, (REAL)middleRect.left, (REAL)(groupRect.top + 1), (REAL)middleRect.left, (REAL)(groupRect.bottom - 2));
    g.DrawLine(&primaryDividerPen, (REAL)rightRect.left, (REAL)(groupRect.top + 1), (REAL)rightRect.left, (REAL)(groupRect.bottom - 2));

    Font textFont(hdc, font);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    RectF leftRectF((REAL)leftRect.left, (REAL)leftRect.top, (REAL)(leftRect.right - leftRect.left), (REAL)(leftRect.bottom - leftRect.top));
    RectF middleRectF((REAL)middleRect.left, (REAL)middleRect.top, (REAL)(middleRect.right - middleRect.left), (REAL)(middleRect.bottom - middleRect.top));
    RectF rightRectF((REAL)rightRect.left, (REAL)rightRect.top, (REAL)(rightRect.right - rightRect.left), (REAL)(rightRect.bottom - rightRect.top));
    g.DrawString(leftText, -1, &textFont, leftRectF, &sf, &textBrush);
    g.DrawString(middleText, -1, &textFont, middleRectF, &sf, &textBrush);
    g.DrawString(rightText, -1, &textFont, rightRectF, &sf, &textBrush);
}
static int MainUI_MeasureTextWidth(HDC hdc, HFONT font, const std::wstring& text) {
    if (text.empty()) {
        return 0;
    }

    HGDIOBJ oldFont = SelectObject(hdc, font);
    SIZE textSize = { 0 };
    GetTextExtentPoint32W(hdc, text.c_str(), (int)text.size(), &textSize);
    SelectObject(hdc, oldFont);
    return textSize.cx;
}
void MainUI_Paint_DrawTextInput(HDC hdc, const RECT& rect, HFONT labelFont, HFONT inputFont, const wchar_t* label, const std::wstring& value, bool isHovering, bool isFocused, size_t* caretPos = nullptr, size_t* viewOffset = nullptr, const wchar_t* placeholder = nullptr) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Font gdiInputFont(hdc, inputFont);
    if (label && label[0] != L'\0') {
        Font gdiLabelFont(hdc, labelFont);
        SolidBrush labelBrush(Color(255, 170, 170, 170));
        g.DrawString(label, -1, &gdiLabelFont, PointF((REAL)rect.left, (REAL)(rect.top - 18)), &labelBrush);
    }

    Color fillColor = isFocused ? Color(140, 55, 55, 55) : (isHovering ? Color(130, 50, 50, 50) : Color(120, 45, 45, 45));
    Color borderColor = isFocused ? Color(180, 0, 122, 204) : Color(180, 80, 80, 80);
    SolidBrush fillBrush(fillColor);
    Pen borderPen(borderColor, 1.0f);

    g.FillRectangle(&fillBrush, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawRectangle(&borderPen, (REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left - 1), (REAL)(rect.bottom - rect.top - 1));

    const int textPaddingX = 12;
    const int textAreaWidth = max(0, rect.right - rect.left - textPaddingX * 2);

    size_t effectiveCaretPos = value.size();
    size_t effectiveViewOffset = 0;
    if (caretPos) {
        effectiveCaretPos = min(*caretPos, value.size());
        *caretPos = effectiveCaretPos;
    }
    if (viewOffset) {
        effectiveViewOffset = min(*viewOffset, value.size());
    }

    if (isFocused && !value.empty()) {
        if (effectiveCaretPos < effectiveViewOffset) {
            effectiveViewOffset = effectiveCaretPos;
        }
        while (effectiveViewOffset < effectiveCaretPos) {
            std::wstring caretWindow = value.substr(effectiveViewOffset, effectiveCaretPos - effectiveViewOffset);
            if (MainUI_MeasureTextWidth(hdc, inputFont, caretWindow) <= textAreaWidth - 2) {
                break;
            }
            ++effectiveViewOffset;
        }
    }
    if (viewOffset) {
        *viewOffset = effectiveViewOffset;
    }

    std::wstring visibleValue = value;
    if (isFocused && effectiveViewOffset < value.size()) {
        visibleValue = value.substr(effectiveViewOffset);
        while (!visibleValue.empty() && MainUI_MeasureTextWidth(hdc, inputFont, visibleValue) > textAreaWidth) {
            visibleValue.pop_back();
        }
    }

    const wchar_t* effectivePlaceholder = placeholder ? placeholder : L"https://discord.com/api/webhooks/...";
    std::wstring displayText = value.empty() ? effectivePlaceholder : visibleValue;
    SolidBrush textBrush(value.empty() ? Color(255, 130, 130, 130) : Color(255, 255, 255, 255));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentNear);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);
    RectF textRect((REAL)(rect.left + textPaddingX), (REAL)rect.top, (REAL)(rect.right - rect.left - textPaddingX * 2), (REAL)(rect.bottom - rect.top));
    g.DrawString(displayText.c_str(), -1, &gdiInputFont, textRect, &sf, &textBrush);

    if (isFocused) {
        std::wstring caretPrefix;
        if (!value.empty() && effectiveCaretPos >= effectiveViewOffset) {
            caretPrefix = value.substr(effectiveViewOffset, effectiveCaretPos - effectiveViewOffset);
        }
        REAL caretX = (REAL)(rect.left + textPaddingX) + min((REAL)textAreaWidth, (REAL)MainUI_MeasureTextWidth(hdc, inputFont, caretPrefix));
        Pen caretPen(Color(255, 255, 255, 255), 1.0f);
        g.DrawLine(&caretPen, caretX, (REAL)rect.top + 8.0f, caretX, (REAL)rect.bottom - 8.0f);
    }
}
void MainUI_Paint_DrawHelpButton(HDC hdc, const RECT& rect, HFONT font, bool isHovering) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush backBrush(isHovering ? Color(140, 80, 80, 80) : Color(140, 55, 55, 55));
    Pen borderPen(Color(180, 90, 90, 90));

    GraphicsPath path;
    REAL r = 5.0f;
    path.AddLine((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - r), (REAL)rect.top);
    path.AddArc((REAL)(rect.right - r * 2), (REAL)rect.top, r * 2, r * 2, 270, 90);
    path.AddArc((REAL)(rect.right - r * 2), (REAL)(rect.bottom - r * 2), r * 2, r * 2, 0, 90);
    path.AddLine((REAL)(rect.right - r), (REAL)rect.bottom, (REAL)rect.left, (REAL)rect.bottom);
    path.CloseFigure();
    g.FillPath(&backBrush, &path);
    g.DrawPath(&borderPen, &path);

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    RectF textRectF((REAL)rect.left, (REAL)rect.top, (REAL)(rect.right - rect.left), (REAL)(rect.bottom - rect.top));
    g.DrawString(L"?", -1, &gdiFont, textRectF, &sf, &textBrush);
}
void MainUI_Paint_DrawTableRow(HDC hdc, const RECT& rowRect, HFONT font, const wchar_t* label, bool isOddRow, const wchar_t* controlValue, bool isControlHovering, bool hasToggle, bool toggleChecked, float toggleAnim, bool hasDropdown, bool hasHelpButton, const RECT& helpButtonRect, bool isHelpHovering, const RECT& dropdownRect, bool isDropdownHovering, const RECT& toggleRect) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    Color rowBgColor = isOddRow ? Color(60, 55, 55, 55) : Color(40, 40, 40, 40);
    SolidBrush rowBrush(rowBgColor);
    g.FillRectangle(&rowBrush, (REAL)rowRect.left, (REAL)rowRect.top, (REAL)(rowRect.right - rowRect.left), (REAL)(rowRect.bottom - rowRect.top));
    Pen sepPen(Color(80, 80, 80, 80), 1.0f);
    g.DrawLine(&sepPen, (REAL)rowRect.left + 10, (REAL)rowRect.bottom, (REAL)rowRect.right, (REAL)rowRect.bottom);

    Font gdiFont(hdc, font);
    SolidBrush textBrush(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)));
    StringFormat sfLeft;
    sfLeft.SetAlignment(StringAlignmentNear);
    sfLeft.SetLineAlignment(StringAlignmentCenter);

    int labelWidth = rowRect.right - rowRect.left - 70;
    int labelMargin = 20;
    RectF labelRectF((REAL)labelMargin, (REAL)rowRect.top, (REAL)labelWidth, (REAL)(rowRect.bottom - rowRect.top));
    g.DrawString(label, -1, &gdiFont, labelRectF, &sfLeft, &textBrush);

    int controlAreaLeft = labelMargin + labelWidth + 5;
    int controlWidth = rowRect.right - controlAreaLeft - (hasHelpButton ? 32 : 10);

    if (hasDropdown) {
        SolidBrush ddBg(isControlHovering ? Color(140, 65, 65, 65) : Color(120, 45, 45, 45));
        Pen ddBorder(Color(180, 80, 80, 80));
        FillRoundedRectangle(&g, &ddBg, (REAL)dropdownRect.left, (REAL)dropdownRect.top, (REAL)(dropdownRect.right - dropdownRect.left), (REAL)(dropdownRect.bottom - dropdownRect.top), 5);
        DrawRoundedRectangle(&g, &ddBorder, (REAL)dropdownRect.left, (REAL)dropdownRect.top, (REAL)(dropdownRect.right - dropdownRect.left), (REAL)(dropdownRect.bottom - dropdownRect.top), 5);

        StringFormat sfDd;
        sfDd.SetAlignment(StringAlignmentNear);
        sfDd.SetLineAlignment(StringAlignmentCenter);
        RectF ddValueRectF((REAL)(dropdownRect.left + 8), (REAL)dropdownRect.top, (REAL)(dropdownRect.right - dropdownRect.left - 25), (REAL)(dropdownRect.bottom - dropdownRect.top));
        g.DrawString(controlValue, -1, &gdiFont, ddValueRectF, &sfDd, &textBrush);

        Pen arrowPen(Color(255, GetRValue(DARK_TEXT), GetGValue(DARK_TEXT), GetBValue(DARK_TEXT)), 2.0f);
        int arrowX = dropdownRect.right - 12;
        int arrowY = dropdownRect.top + (dropdownRect.bottom - dropdownRect.top) / 2;
        g.DrawLine(&arrowPen, (REAL)(arrowX - 4), (REAL)(arrowY - 2), (REAL)arrowX, (REAL)(arrowY + 2));
        g.DrawLine(&arrowPen, (REAL)arrowX, (REAL)(arrowY + 2), (REAL)(arrowX + 4), (REAL)(arrowY - 2));
    }
    else if (hasToggle) {
        int toggleW = 50, toggleH = 24;
        int toggleX = toggleRect.right - toggleW;
        int toggleY = toggleRect.top + (toggleRect.bottom - toggleRect.top - toggleH) / 2 + 4;
        RECT toggleBackRect = { toggleX, toggleY, toggleX + toggleW, toggleY + toggleH };

        SolidBrush toggleBrush(toggleChecked ? Color(255, 0, 122, 204) : Color(140, 80, 80, 80));
        Pen toggleBorderPen(toggleChecked ? Color(255, 0, 122, 204) : Color(180, 80, 80, 80), toggleChecked ? 1.5f : 1.0f);

        GraphicsPath path;
        REAL r = 6.0f;
        path.AddArc((REAL)toggleBackRect.left, (REAL)toggleBackRect.top, r * 2, r * 2, 180, 90);
        path.AddLine((REAL)(toggleBackRect.left + r), (REAL)toggleBackRect.top, (REAL)toggleBackRect.right, (REAL)toggleBackRect.top);
        path.AddLine((REAL)toggleBackRect.right, (REAL)toggleBackRect.top, (REAL)toggleBackRect.right, (REAL)toggleBackRect.bottom);
        path.AddLine((REAL)toggleBackRect.right, (REAL)toggleBackRect.bottom, (REAL)(toggleBackRect.left + r), (REAL)toggleBackRect.bottom);
        path.AddArc((REAL)toggleBackRect.left, (REAL)(toggleBackRect.bottom - r * 2), r * 2, r * 2, 90, 90);
        path.CloseFigure();
        g.FillPath(&toggleBrush, &path);
        g.DrawPath(&toggleBorderPen, &path);

        SolidBrush knobBrush(Color(255, 255, 255, 255));
        int knobSize = toggleH - 8;
        int startX = toggleBackRect.left + 4;
        int endX = toggleBackRect.right - knobSize - 4;
        int knobX = startX + (int)((endX - startX) * toggleAnim);
        int knobY = toggleBackRect.top + 4;
        g.FillEllipse(&knobBrush, knobX, knobY, knobSize, knobSize);
    }
    else if (!hasToggle && !hasDropdown && controlValue != nullptr && controlValue[0] != 0) {
        int valueRightMargin = 20;
        int valueWidth = 160;
        int valueX = rowRect.right - valueWidth - valueRightMargin;
        RectF valueRect((REAL)valueX, (REAL)rowRect.top, (REAL)valueWidth, (REAL)(rowRect.bottom - rowRect.top));
        StringFormat sfValue;
        sfValue.SetAlignment(StringAlignmentFar);
        sfValue.SetLineAlignment(StringAlignmentCenter);
        SolidBrush valueBrush(Color(255, 255, 255, 255));
        g.DrawString(controlValue, -1, &gdiFont, valueRect, &sfValue, &valueBrush);
    }

    if (hasHelpButton) {
        MainUI_Paint_DrawHelpButton(hdc, helpButtonRect, font, isHelpHovering);
    }
}
void MainUI_HandleClick(HWND hwnd, POINT pt, MainUIData* pData) {
    for (int i = 0; i < pData->navItems.size(); ++i) {
        if (PtInRect(&pData->navItems[i].rect, pt)) {
            pData->isDiscordWebhookInputFocused = false;
            pData->currentPage = i;
            InvalidateRect(hwnd, NULL, FALSE);
            return;
        }
    }

    bool clickedDiscordInput = (pData->currentPage == 5 && PtInRect(&pData->discordWebhookInputRect, pt));
    if (pData->isDiscordWebhookInputFocused && !clickedDiscordInput) {
        pData->isDiscordWebhookInputFocused = false;
        InvalidateRect(hwnd, &pData->discordWebhookInputRect, FALSE);
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

    if (pData->currentPage == 0 && PtInRect(&pData->discordButtonRect, pt)) {
        ShellExecute(NULL, L"open", L"https://agzes.github.io/go/to/discord", NULL, NULL, SW_SHOWNORMAL);
        return;
    }

    if (pData->currentPage == 0 && PtInRect(&pData->importSettingsRect, pt)) {
        PostMessage(g_hwnd, WM_COMMAND, ID_IMPORT_SETTINGS, 0);
        return;
    }

    if (pData->currentPage == 0 && PtInRect(&pData->exportSettingsRect, pt)) {
        PostMessage(g_hwnd, WM_COMMAND, ID_EXPORT_SETTINGS, 0);
        return;
    }

    if (pData->currentPage == 0 && g_updateFound.load() && PtInRect(&pData->updateBannerRect, pt)) {
        PostMessage(g_hwnd, WM_COMMAND, ID_UPDATE_AVAILABLE, 0);
        return;
    }

    auto toggleDiscordFlag = [&](std::atomic<bool>& flag) {
        flag = !flag.load();
        if (&flag == &g_discordNotifyStart) {
            g_discordNotifyStop = g_discordNotifyStart.load();
        }
        SaveSettings();
        InvalidateRect(hwnd, NULL, FALSE);
        };

    if (pData->currentPage == 5) {
        if (PtInRect(&pData->discordWebhookEnableToggleRect, pt)) {
            toggleDiscordFlag(g_discordWebhookEnabled);
            return;
        }
        if (PtInRect(&pData->discordWebhookInputRect, pt)) {
            pData->isDiscordWebhookInputFocused = true;
            pData->discordWebhookCaretPos = GetDiscordWebhookUrlCopy().size();
            pData->discordWebhookViewOffset = min(pData->discordWebhookViewOffset, pData->discordWebhookCaretPos);
            SetFocus(hwnd);
            InvalidateRect(hwnd, &pData->discordWebhookInputRect, FALSE);
            return;
        }

        if (PtInRect(&pData->discordWebhookPasteRect, pt)) {
            std::wstring clipboardText;
            if (!ReadUnicodeTextFromClipboard(hwnd, clipboardText)) {
                ShowDarkMessageBox(hwnd, L"Clipboard does not contain Unicode text right now.", L"Discord Webhook", MB_OK | MB_ICONWARNING);
                return;
            }

            SetDiscordWebhookUrl(clipboardText);
            SaveSettings();
            pData->isDiscordWebhookInputFocused = true;
            pData->discordWebhookCaretPos = GetDiscordWebhookUrlCopy().size();
            pData->discordWebhookViewOffset = pData->discordWebhookCaretPos;
            SetFocus(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            return;
        }

        if (PtInRect(&pData->discordWebhookClearRect, pt)) {
            SetDiscordWebhookUrl(L"");
            SaveSettings();
            pData->isDiscordWebhookInputFocused = true;
            pData->discordWebhookCaretPos = 0;
            pData->discordWebhookViewOffset = 0;
            SetFocus(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            return;
        }

        if (PtInRect(&pData->discordWebhookTestRect, pt)) {
            std::wstring webhookUrl = GetDiscordWebhookUrlCopy();
            if (!IsDiscordWebhookUrl(webhookUrl)) {
                ShowDarkMessageBox(hwnd, L"Enter a valid Discord webhook URL first.", L"Discord Webhook", MB_OK | MB_ICONWARNING);
                return;
            }

            std::wstring errorText;
            DWORD statusCode = 0;
            bool sent = SendDiscordWebhookRequest(
                webhookUrl,
                DiscordWebhookEvent::Test,
                L"Manual test message from the Discord tab.",
                &errorText,
                &statusCode);

            if (sent) {
                wchar_t successMessage[96];
                swprintf_s(successMessage, L"Test message sent successfully (HTTP %lu).", statusCode);
                ShowDarkMessageBox(hwnd, successMessage, L"Discord Webhook", MB_OK | MB_ICONINFORMATION);
            } else {
                ShowDarkMessageBox(hwnd, errorText.empty() ? L"Failed to send the test webhook." : errorText.c_str(), L"Discord Webhook", MB_OK | MB_ICONERROR);
            }
            return;
        }

        if (PtInRect(&pData->discordNotifyStartToggleRect, pt)) { toggleDiscordFlag(g_discordNotifyStart); return; }
        if (PtInRect(&pData->discordNotifyActionToggleRect, pt)) { toggleDiscordFlag(g_discordNotifyAction); return; }
        if (PtInRect(&pData->discordNotifyReconnectToggleRect, pt)) { toggleDiscordFlag(g_discordNotifyReconnect); return; }
        if (PtInRect(&pData->discordNotifyErrorsToggleRect, pt)) { toggleDiscordFlag(g_discordNotifyErrors); return; }
        if (PtInRect(&pData->discordDisableEmbedToggleRect, pt)) { toggleDiscordFlag(g_discordDisableEmbed); return; }
        if (PtInRect(&pData->discordMentionOnErrorsToggleRect, pt)) { toggleDiscordFlag(g_discordMentionOnErrors); return; }
    }

    for (int i = 0; i < pData->helpButtonRects.size(); ++i) {
        if (PtInRect(&pData->helpButtonRects[i], pt)) {
            const wchar_t* title = L"Help";
            const wchar_t* text = L"No help text available for this setting.";
            if (pData->currentPage == 0) { // General
                if (i == 0) { title = L"Interval"; text = L"Sets how often AntiAFK-RBX will do an action in Roblox so you are not marked as idle.\n\nRoblox usually kicks for inactivity after about 20 minutes, so this value should stay below that limit.\n\nRecommended: 9 minutes.\n\nA shorter interval means more frequent protection. A longer interval means fewer actions, but if it is too close to 20 minutes there is a higher chance of an AFK kick."; }
                if (i == 1) { title = L"Action"; text = L"Selects which input AntiAFK-RBX will simulate to keep your Roblox session active.\n\nUse the option that behaves most naturally in your game. If one action is ignored or causes unwanted behavior, switch to another and test again."; }
                if (i == 2) { title = L"Multi-Instance Support"; text = L"Enables Anti-AFK protection for multiple Roblox windows at the same time.\n\nWhen this is on, the app cycles through every detected Roblox client and performs the selected anti-idle action for each one.\n\nRecommended setup:\n1. Close all Roblox clients\n2. Open AntiAFK-RBX\n3. Turn on Multi-Instance Support\n4. Launch the Roblox clients you want to keep active"; }
                if (i == 3) { title = L"Multi-Instance Interval"; text = L"Adds a delay between handling each Roblox window while Multi-Instance Support is enabled.\n\nUse 'Minimum' for the fastest switching speed. Increase the delay if windows are skipped, focus changes feel unstable, or Roblox does not react reliably on your PC."; }
                if (i == 4) { title = L"User-Safe Mode"; text = L"Prevents AntiAFK-RBX from acting while you are actively using the computer.\n\nThis helps avoid interruptions when you are typing, clicking, or controlling the system yourself. The app waits until your input stops, then performs the anti-idle action.\n\nLegacy: checks keyboard and mouse activity.\nBeta: checks wider system input and is usually more reliable."; }
                if (i == 5) { title = L"Restore Window"; text = L"Chooses how AntiAFK-RBX returns focus after briefly interacting with Roblox.\n\nSetForeground is the recommended default and works best for normal desktop use.\nAlt+Tab can work better in some fullscreen situations, but it is more intrusive because it simulates task switching.\nOff leaves focus behavior unchanged."; }
            } else if (pData->currentPage == 1) { // Automation
                if (i == 0) { title = L"Auto-Start AntiAFK"; text = L"Automatically turns Anti-AFK on when Roblox is detected and turns it off again when Roblox closes.\n\nUse this if you want the protection to manage itself without pressing Start each time."; }
                if (i == 1) { title = L"Auto Reconnect*"; text = L"Experimental feature (*) that tries to press Roblox's reconnect button after an idle kick or similar disconnect.\n\nThis is useful for unattended sessions, but it depends on the reconnect screen being visible and detected correctly.\n\nNote: sometimes it may fail to click the reconnect button even when the screen is shown."; }
                if (i == 2) { title = L"Auto Reset"; text = L"During each Anti-AFK cycle, the app sends the reset sequence: Esc, R, Enter.\n\nThis can respawn your character instead of only simulating a small movement or key press. Use it only in games where resetting is safe and intentional."; }
                if (i == 3) { title = L"Auto-Hide Roblox"; text = L"Hides Roblox windows when Anti-AFK starts so they can run in the background without staying visible on screen.\n\nThis hides the windows instead of minimizing them. You can show them again later from the tray controls or the Utils tab."; }
                if (i == 4) { title = L"Auto Opacity"; text = L"Automatically applies the 70% Roblox window opacity effect while AntiAFK-RBX is managing your session.\n\nUseful when new Roblox windows appear later and you want them to match the same translucent look without toggling opacity manually each time."; }
                if (i == 5) { title = L"Auto Grid*"; text = L"Experimental feature (*) that automatically arranges Roblox windows into the grid layout when the set of detected Roblox windows changes.\n\nThis is especially helpful for multi-instance setups because newly opened clients will be snapped into place without using the tray action manually.\n\nNote: in rare cases it may bug or misplace windows. If that happens, disable it and use the manual Grid Snap action."; }
                if (i == 6) { title = L"Fish/Void/Bloxstrap Integration"; text = L"When enabled, AntiAFK-RBX run and starts automatically when Roblox is launched through Fishstrap, Voidstrap, or Bloxstrap."; }
                if (i == 7) { title = L"I can forget (smart auto-start)"; text = L"A safety net for people who often forget to start Anti-AFK manually.\n\nWhile Roblox is open, the app watches for long inactivity. Around 18 minutes, it shows a reminder. Around 19 minutes, it starts Anti-AFK automatically to help avoid the 20-minute idle kick."; }
                if (i == 8) { title = L"Do not sleep"; text = L"Stops Windows from putting the PC to sleep while Anti-AFK is active.\n\nUseful for long unattended sessions. On laptops, this can also prevent sleep-related behavior that would otherwise interrupt Roblox."; }
            } else if (pData->currentPage == 3) { // Advanced
                if (i == 0) { title = L"FishStrap/Shaders Support"; text = L"Legacy compatibility option.\n\nThis was mainly needed back when Fishstrap/shader-based setups were commonly used and Roblox detection could break.\n\nToday it usually has no benefit, but it is kept for older/edge setups. Leave it off unless you specifically need it."; }
                if (i == 1) { title = L"Update Checker"; text = L"Checks for newer AntiAFK-RBX versions when the app starts and shows a notice if an update is available.\n\nDisable this only if you prefer to check for updates manually."; }
                if (i == 2) { title = L"Use Legacy UI (Tray)"; text = L"Switches from the modern window interface to the older tray-menu style interface.\n\nThis is useful if you prefer the classic compact workflow. You can switch back later from the tray settings."; }
                if (i == 3) { title = L"Status Bar*"; text = L"Experimental feature (*) that shows a small on-screen overlay for important events such as start, stop, reconnect actions, and other Anti-AFK status updates.\n\nThis is just an experimental UI feature. Disable it if you want a quieter experience with tray notifications only."; }
                if (i == 4) { title = L"FPS Capper*"; text = L"Experimental feature (*) that reduces Roblox's frame rate to lower CPU and GPU usage.\n\nThis can save power and reduce heat during background sessions.\n\nNote: it works by limiting/influencing the Roblox process timing rather than enforcing an exact in-game FPS lock, so the FPS you see may fluctuate."; }
                if (i == 5) { title = L"Unlock FPS when focus"; text = L"Temporarily disables the FPS cap for the Roblox window you are currently using.\n\nThis lets you keep low resource usage in the background while still getting normal responsiveness when you return to the game."; }
                if (i == 6) { title = L"AutoMute Roblox"; text = L"Mutes Roblox automatically when Anti-AFK starts and restores audio when Anti-AFK stops.\n\nUseful if Roblox is running in the background and you do not want game sounds playing the whole time."; }
                if (i == 7) { title = L"Unmute on focus"; text = L"Temporarily unmutes Roblox when you click back into a Roblox window, then mutes it again when focus is lost.\n\nThis works best together with AutoMute Roblox."; }
            } else if (pData->currentPage == 4) { // Utils
                if (i == 0) { title = L"Window Opacity"; text = L"Temporarily enables or disables the Roblox window opacity effect for this session only.\n\nThis does not change the saved setting from the other tabs and resets when the app is restarted."; }
                if (i == 1) { title = L"Mute/UnMute Roblox"; text = L"Temporarily forces Roblox audio on or off for the current session.\n\nThis override does not save to settings and takes priority over the normal AutoMute behavior until the app is restarted."; }
                if (i == 2) { title = L"Limit/Unlimit FPS*"; text = L"Experimental feature (*) that temporarily enables a session-only FPS limit for Roblox.\n\nThis Utils toggle works even if AntiAFK has not been started.\n\nWhen enabled, it uses the FPS value from Advanced -> FPS Capper. If Advanced FPS Capper is Off, it falls back to 5 FPS for this session.\n\nIf Unlock FPS when focus is enabled in Advanced, that behavior still applies here too.\n\nNote: the FPS limiting behavior works by influencing Roblox process timing, so the FPS you see may fluctuate.\n\nWhen you turn it off again, control returns to the normal Advanced -> FPS Capper setting.\n\nThis override does not change the saved Advanced setting and resets when AntiAFK-RBX is restarted."; }
            } else if (pData->currentPage == 5) { // Discord
                if (i == 0) { title = L"Enable webhook"; text = L"Master switch for all Discord webhook sending.\n\nTurn this off if you want to keep the URL and options saved, but temporarily stop all webhook traffic."; }
                if (i == 1) { title = L"Start / Stop"; text = L"Sends a Discord notification when AntiAFK starts and another when it stops.\n\nGood for basic remote monitoring without too many messages."; }
                if (i == 2) { title = L"Action"; text = L"Sends a Discord notification after every Anti-AFK cycle.\n\nThis provides the most detailed activity log, but it can create a lot of messages if your interval is short."; }
                if (i == 3) { title = L"Reconnect"; text = L"Sends a Discord notification whenever Auto Reconnect attempts to press the reconnect button.\n\nUseful if you want to know when Roblox needed recovery while you were away."; }
                if (i == 4) { title = L"Errors"; text = L"Sends a Discord notification for important problems, such as Roblox not being found or a feature failing to run.\n\nRecommended if you want to be alerted only when something needs attention."; }
                if (i == 5) { title = L"Disable embed"; text = L"Sends Discord webhook messages as plain one-line text instead of rich embeds.\n\nUseful for simple log channels, mobile previews, or webhook destinations where embeds are filtered or look too large."; }
                if (i == 6) { title = L"Mention on errors"; text = L"Adds an @everyone mention only for Error and Reconnect webhook events.\n\nUseful if you want the whole channel to notice failures without pinging on normal status updates."; }
            }
            ShowDarkMessageBox(hwnd, text, title, MB_OK | MB_ICONINFORMATION);
            return;
        }
    }

    if (pData->currentPage == 0 && PtInRect(&pData->multiInstanceToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_MULTI_SUPPORT, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoStartToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_START_AFK, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoReconnectToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_RECONNECT, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoResetToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_RESET, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoHideToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_HIDE, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoOpacityToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_OPACITY, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->autoGridToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_GRID, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->doNotSleepToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_DO_NOT_SLEEP, 0); return; }

    if (pData->currentPage == 3 && PtInRect(&pData->unlockFpsOnFocusToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UNLOCK_FPS_ON_FOCUS, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->autoMuteToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_MUTE, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->unmuteOnFocusToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UNMUTE_ON_FOCUS, 0); return; }
    if (pData->currentPage == 1 && PtInRect(&pData->bloxstrapIntegrationToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_BLOXSTRAP_INTEGRATION, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->fishstrapToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_FISHSTRAP_SUP, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->autoUpdateToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_AUTO_UPDATE, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->statusBarToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_STATUS_BAR, 0); return; }
    if (pData->currentPage == 3 && PtInRect(&pData->legacyUiToggleRect, pt)) {
        int result = ShowDarkMessageBox(hwnd, L"Are you sure you want to switch to the legacy tray UI?\n\nThe modern UI window will be closed. To open it again, you will need to disable 'Use Legacy UI' from the tray menu and restart the application.", L"Switch to Legacy UI?", MB_YESNO | MB_ICONQUESTION);
        if (result == IDYES) {
            PostMessage(g_hwnd, WM_COMMAND, ID_USE_LEGACY_UI, 0);
            DestroyWindow(hwnd);
        }
        return;
    }
    if (pData->currentPage == 1 && PtInRect(&pData->iCanForgetToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_I_CAN_FORGET, 0); return; }


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
        if (PtInRect(&pData->multiInstanceIntervalDropdownRect, pt)) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | (g_multiInstanceInterval == 0 ? MF_CHECKED : 0), ID_MI_INTERVAL_0, L"Minimum (0)");
            AppendMenu(hMenu, MF_STRING | (g_multiInstanceInterval == 1000 ? MF_CHECKED : 0), ID_MI_INTERVAL_1, L"1 sec");
            AppendMenu(hMenu, MF_STRING | (g_multiInstanceInterval == 3000 ? MF_CHECKED : 0), ID_MI_INTERVAL_3, L"3 sec");
            AppendMenu(hMenu, MF_STRING | (g_multiInstanceInterval == 5000 ? MF_CHECKED : 0), ID_MI_INTERVAL_5, L"5 sec");
            AppendMenu(hMenu, MF_STRING | (g_multiInstanceInterval == 10000 ? MF_CHECKED : 0), ID_MI_INTERVAL_10, L"10 sec");
            POINT menuPt;
            menuPt.x = pData->multiInstanceIntervalDropdownRect.left;
            menuPt.y = pData->multiInstanceIntervalDropdownRect.bottom;
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

    if (pData->currentPage == 4) {
        if (PtInRect(&pData->utilsShowRobloxRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_SHOW_ALL, 0); return; }
        if (PtInRect(&pData->utilsHideRobloxRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_HIDE_ALL, 0); return; }
        if (PtInRect(&pData->utilsGridSnapRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_GRID_SNAP, 0); return; }
        if (PtInRect(&pData->utilsWindowOpacityRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_TOGGLE_WINDOW_OPACITY, 0); return; }
        if (PtInRect(&pData->utilsCloseAllRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_CLOSE_ALL, 0); return; }
        if (PtInRect(&pData->utilsResetAllRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_RESET_ALL, 0); return; }
        if (PtInRect(&pData->utilsTestActionRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_TEST_ACTION, 0); return; }
        if (PtInRect(&pData->utilsMuteToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_TOGGLE_MUTE, 0); return; }
        if (PtInRect(&pData->utilsFpsToggleRect, pt)) { PostMessage(g_hwnd, WM_COMMAND, ID_UTILS_TOGGLE_FPS, 0); return; }
    }
}
void MainUI_Paint_DrawContent(HDC hdc, const RECT& clientRect, MainUIData* pData) 
{
    bool needsRedraw = false;
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
            needsRedraw = true;
        } else {
            val = targetVal;
        }
    };

    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

    pData->closeButtonRect = { clientRect.right - 46, 0, clientRect.right, 30 };
    pData->iconButtonRect = { 0, 0, 46, 30 };

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

    if (pData->isHoveringClose) {
        SolidBrush closeHoverBrush(Color(255, 232, 17, 35));
        g.FillRectangle(&closeHoverBrush, (REAL)pData->closeButtonRect.left, (REAL)pData->closeButtonRect.top, (REAL)(pData->closeButtonRect.right - pData->closeButtonRect.left), (REAL)(pData->closeButtonRect.bottom - pData->closeButtonRect.top));
    }
    Pen closePen(Color(255, 255, 255, 255), 1.0f);
    int cx = pData->closeButtonRect.left + 23;
    int cy = 15;
    g.DrawLine(&closePen, cx - 5, cy - 5, cx + 5, cy + 5);
    g.DrawLine(&closePen, cx + 5, cy - 5, cx - 5, cy + 5);

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
    SolidBrush startBrush(Color(100, r, gr, b));

    g.FillRectangle(&startBrush, (REAL)pData->startButtonRect.left, (REAL)pData->startButtonRect.top, (REAL)(pData->startButtonRect.right - pData->startButtonRect.left), (REAL)(pData->startButtonRect.bottom - pData->startButtonRect.top));
    
    HFONT oldFont2 = (HFONT)SelectObject(hdc, pData->hFontSection);
    StringFormat sfStart;
    sfStart.SetAlignment(StringAlignmentCenter);
    sfStart.SetLineAlignment(StringAlignmentCenter);
    RectF startTextRectF((REAL)pData->startButtonRect.left, (REAL)pData->startButtonRect.top, (REAL)(pData->startButtonRect.right - pData->startButtonRect.left), (REAL)(pData->startButtonRect.bottom - pData->startButtonRect.top));
    Font gdiSectionFont2(hdc, pData->hFontSection);

    if (stateProgress < 1.0f) {
        SolidBrush prevTextBrush(Color((BYTE)(255 * (1.0f - stateProgress) * (1.0f - progress)), 255, 255, 255));
        g.DrawString(prevText, -1, &gdiSectionFont2, startTextRectF, &sfStart, &prevTextBrush);
    }
    if (stateProgress > 0.0f) {
        SolidBrush currentTextBrush(Color((BYTE)(255 * stateProgress * (1.0f - progress)), 255, 255, 255));
        g.DrawString(startText, -1, &gdiSectionFont2, startTextRectF, &sfStart, &currentTextBrush);
    }

    if (progress > 0.0f && !pData->startButtonErrorText.empty()) {
        SolidBrush errorTextBrush(Color((BYTE)(255 * progress), 255, 255, 255));
        g.DrawString(pData->startButtonErrorText.c_str(), -1, &gdiSectionFont2, startTextRectF, &sfStart, &errorTextBrush);
    }

    SelectObject(hdc, oldFont2);

    RECT navBarRect = { 0, 30, clientRect.right, 60 };
    SolidBrush navBrush(Color(100, 35, 35, 35));
    g.FillRectangle(&navBrush, (REAL)navBarRect.left, (REAL)navBarRect.top, (REAL)(navBarRect.right - navBarRect.left), (REAL)(navBarRect.bottom - navBarRect.top));

    const int margin = 20, rowH = 32, vGap = 10;
    const int nav_bar_y = 30, nav_bar_h = 30;
    const int startBtnH = 40;
    const int content_y_start = nav_bar_y + nav_bar_h;
    const int disclaimerH = 26;
    int help_btn_size = 24;
    int ctrlStartX = margin;
    int ctrlEndX = clientRect.right - 20;
    int y = content_y_start;

    pData->helpButtonRects.clear();
    pData->rowRects.clear();
    
    pData->startButtonRect = { 0, clientRect.bottom - startBtnH, clientRect.right, clientRect.bottom };
    pData->disclaimerRect = { 0, clientRect.bottom - startBtnH - disclaimerH + 1, clientRect.right, clientRect.bottom - startBtnH + 1 };

    if (pData->currentPage == 0) { // General
        int ddW_interval = 166, ddW_action = 144, ddW_mi_interval = 143, ddW_user_safe = 181, ddW_restore = 161;
        
        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->intervalDropdownRect = { ctrlEndX - ddW_interval - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->actionDropdownRect = { ctrlEndX - ddW_action - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->multiInstanceToggleRect = { ctrlStartX, y, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->multiInstanceIntervalDropdownRect = { ctrlEndX - ddW_mi_interval - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->userSafeDropdownRect = { ctrlEndX - ddW_user_safe - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->restoreMethodDropdownRect = { ctrlEndX - ddW_restore - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->discordButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
        pData->updateBannerRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH * 2, clientRect.right, clientRect.bottom - startBtnH - disclaimerH - rowH };
    } else if (pData->currentPage == 1) { // Automation
        int toggleW = ctrlEndX - ctrlStartX - help_btn_size;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoStartToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoReconnectToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoResetToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoHideToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoOpacityToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoGridToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->bloxstrapIntegrationToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->iCanForgetToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->doNotSleepToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;
    } else if (pData->currentPage == 2) { // Statistics
        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        y += rowH + vGap;

        pData->resetStatsButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
    } else if (pData->currentPage == 3) { // Advanced
        int ddW_fps = 173;
        int ctrlW = ctrlEndX - ctrlStartX - help_btn_size;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->fishstrapToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoUpdateToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->legacyUiToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->statusBarToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->fpsCapperDropdownRect = { ctrlEndX - ddW_fps - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->unlockFpsOnFocusToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->autoMuteToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->unmuteOnFocusToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->resetSettingsButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
        } else if (pData->currentPage == 5) { // Discord
            const int noteH = 26;
            const int buttonGap = 0;
            int toggleW = ctrlEndX - ctrlStartX - help_btn_size;
            int buttonW = clientRect.right / 3;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordWebhookEnableToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->discordPayloadInfoRect = { 0, y, clientRect.right, y + noteH };
            y += noteH - 1;

            pData->discordWebhookInputRect = { 0, y, clientRect.right, y + rowH };
            y += rowH - 1;

            pData->discordWebhookPasteRect = { 0, y, buttonW, y + rowH };
            pData->discordWebhookClearRect = { pData->discordWebhookPasteRect.right + buttonGap, y, pData->discordWebhookPasteRect.right + buttonGap + buttonW, y + rowH };
            pData->discordWebhookTestRect = { pData->discordWebhookClearRect.right + buttonGap, y, clientRect.right, y + rowH };
            y += rowH;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordNotifyStartToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordNotifyActionToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordNotifyReconnectToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordNotifyErrorsToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordDisableEmbedToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->discordMentionOnErrorsToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
    } else if (pData->currentPage == 4) { // Utils
        int toggleCtrlW = ctrlEndX - ctrlStartX - help_btn_size;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsShowRobloxRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsHideRobloxRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsGridSnapRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsWindowOpacityRect = { ctrlStartX, y, ctrlStartX + toggleCtrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsCloseAllRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsResetAllRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsTestActionRect = { 0, y, clientRect.right, y + rowH + vGap };
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsMuteToggleRect = { ctrlStartX, y, ctrlStartX + toggleCtrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->utilsFpsToggleRect = { ctrlStartX, y, ctrlStartX + toggleCtrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
    }

    const int total_nav_w = clientRect.right;
    const int collapsed_w = 48;

    const bool isHoveringAnotherTab = pData->hoveringNavItem != -1 && pData->hoveringNavItem != pData->currentPage;
    auto shouldExpandNavItem = [&](size_t index) {
        if (pData->hoveringNavItem != -1) {
            return pData->hoveringNavItem == (int)index;
        }
        return pData->currentPage == (int)index;
    };

    int num_expanded = 0;
    for (size_t i = 0; i < pData->navItems.size(); ++i) {
        if (shouldExpandNavItem(i)) {
            num_expanded++;
        }
    }

    int expanded_w = num_expanded > 0 ? (total_nav_w - (int)(pData->navItems.size() - num_expanded) * collapsed_w) / num_expanded : total_nav_w / (int)pData->navItems.size();

    int current_x = 0;
    for (size_t i = 0; i < pData->navItems.size(); ++i) {
        int target_w = shouldExpandNavItem(i) ? expanded_w : collapsed_w;

        if (pData->navItems[i].currentWidth == 0.0f) pData->navItems[i].currentWidth = (float)target_w;
        
        if (abs(pData->navItems[i].currentWidth - (float)target_w) > 0.5f) {
            pData->navItems[i].currentWidth += ((float)target_w - pData->navItems[i].currentWidth) * animSpeed;
            needsRedraw = true;
        } else {
            pData->navItems[i].currentWidth = (float)target_w;
        }

        int draw_w = (int)pData->navItems[i].currentWidth;
        if (i == pData->navItems.size() - 1) draw_w = total_nav_w - current_x;

        pData->navItems[i].rect = { current_x, nav_bar_y, current_x + draw_w, nav_bar_y + nav_bar_h };
        current_x += draw_w;
    }

    for (size_t i = 0; i < pData->navItems.size(); ++i) {
        bool isSelected = (pData->currentPage == i);
        bool isHovering = (pData->hoveringNavItem == i);
        bool isExpanded = shouldExpandNavItem(i);

        HFONT fontToUse = pData->hFontNav;
        Color textColor(255, 160, 160, 160);

        if (isSelected) {
            fontToUse = pData->hFontNavSelected;
            textColor = isHoveringAnotherTab ? Color(255, 220, 220, 220) : Color(255, 255, 255, 255);
        } else if (isHovering) {
            textColor = Color(255, 230, 230, 230);
        }

        Font navFont(hdc, fontToUse);
        SolidBrush navBrush(textColor);
        RectF navItemRect((REAL)pData->navItems[i].rect.left, (REAL)pData->navItems[i].rect.top, (REAL)(pData->navItems[i].rect.right - pData->navItems[i].rect.left), (REAL)(pData->navItems[i].rect.bottom - pData->navItems[i].rect.top));

        StringFormat sfIcon;
        sfIcon.SetAlignment(StringAlignmentCenter);
        sfIcon.SetLineAlignment(StringAlignmentCenter);

        Font navIconFont(hdc, pData->hFontNavIcon);

        if (isExpanded && pData->navItems[i].currentWidth > collapsed_w + 10) {
            RectF iconRect = navItemRect;
            iconRect.Width = (REAL)collapsed_w;
            g.DrawString(pData->navItems[i].icon.c_str(), -1, &navIconFont, iconRect, &sfIcon, &navBrush);

            RectF textRect = navItemRect;
            textRect.X += (REAL)collapsed_w;
            textRect.Width -= (REAL)collapsed_w;

            StringFormat sfText;
            sfText.SetAlignment(StringAlignmentNear);
            sfText.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(pData->navItems[i].title.c_str(), -1, &navFont, textRect, &sfText, &navBrush);
        } else {
            g.DrawString(pData->navItems[i].icon.c_str(), -1, &navIconFont, navItemRect, &sfIcon, &navBrush);
        }
    }
    SolidBrush underlineTrackBrush(Color(90, 65, 65, 65));
    RECT navTrackRect = { 0, navBarRect.bottom - 1, clientRect.right, navBarRect.bottom };
    g.FillRectangle(&underlineTrackBrush, (REAL)navTrackRect.left, (REAL)navTrackRect.top, (REAL)(navTrackRect.right - navTrackRect.left), (REAL)(navTrackRect.bottom - navTrackRect.top));

    SolidBrush underlineBrush(Color(255, 0, 122, 204));
    RECT navUnderlineRect = { (int)pData->navIndicatorX, navBarRect.bottom - 1, (int)(pData->navIndicatorX + pData->navIndicatorWidth), navBarRect.bottom };
    g.FillRectangle(&underlineBrush, (REAL)navUnderlineRect.left, (REAL)navUnderlineRect.top, (REAL)(navUnderlineRect.right - navUnderlineRect.left), (REAL)(navUnderlineRect.bottom - navUnderlineRect.top));


    const int content_x_start = 20;
    if (pData->currentPage == 0) { // General
        int rowCount = (int)pData->rowRects.size();
        int rowLeft = 0;
        int rowRight = clientRect.right;
        for (int i = 0; i < rowCount; i++) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rowLeft, (REAL)rr.top, (REAL)(rowRight - rowLeft), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rowLeft, (REAL)rr.bottom - 1, (REAL)rowRight, (REAL)rr.bottom - 1);
        }

        wchar_t buffer[64];
        swprintf_s(buffer, L"%d sec (%d min)", g_selectedTime, g_selectedTime / 60);
        MainUI_Paint_DrawDropdown(hdc, pData->intervalDropdownRect, pData->hFontText, L"Interval", buffer, pData->isHoveringInterval, true, L"\uE916");

        const wchar_t* actionNames[] = { L"Space (Jump)", L"W/S", L"Zoom (I/O)" };
        MainUI_Paint_DrawDropdown(hdc, pData->actionDropdownRect, pData->hFontText, L"Action", actionNames[g_selectedAction], pData->isHoveringAction, true, L"\uE7C9");

        const wchar_t* userSafeNames[] = { L"Off", L"Legacy", L"Beta" };
        MainUI_Paint_DrawDropdown(hdc, pData->userSafeDropdownRect, pData->hFontText, L"User-Safe Mode", userSafeNames[g_userSafeMode], pData->isHoveringUserSafe, true, L"\uE72E");

        const wchar_t* restoreNames[] = { L"Off", L"SetForeground", L"Alt+Tab" };
        MainUI_Paint_DrawDropdown(hdc, pData->restoreMethodDropdownRect, pData->hFontText, L"Restore Window", restoreNames[g_restoreMethod], pData->isHoveringRestore, true, L"\uE73F");

        MainUI_Paint_DrawToggle(hdc, pData->multiInstanceToggleRect, pData->hFontText, L"Multi-Instance Support", g_multiSupport.load(), PtInRect(&pData->multiInstanceToggleRect, pData->hoverPoint), pData->multiInstanceAnim, true, L"\uE81E");

        const wchar_t* miIntervalNames[] = { L"Minimum", L"1 sec", L"3 sec", L"5 sec", L"10 sec" };
        int miIntervalIndex = 0;
        if (g_multiInstanceInterval == 1000) miIntervalIndex = 1;
        else if (g_multiInstanceInterval == 3000) miIntervalIndex = 2;
        else if (g_multiInstanceInterval == 5000) miIntervalIndex = 3;
        else if (g_multiInstanceInterval == 10000) miIntervalIndex = 4;
        MainUI_Paint_DrawDropdown(hdc, pData->multiInstanceIntervalDropdownRect, pData->hFontText, L"Multi-Instance Interval", miIntervalNames[miIntervalIndex], pData->isHoveringMultiInstanceInterval, true, L"\uE916");

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }

        SolidBrush backupInfoBgBrush(Color(30, 35, 35, 35));
        Pen backupInfoBorderPen(Color(60, 70, 70, 70), 1.0f);
        g.FillRectangle(&backupInfoBgBrush, (REAL)pData->backupMessageRect.left, (REAL)pData->backupMessageRect.top, (REAL)(pData->backupMessageRect.right - pData->backupMessageRect.left), (REAL)(pData->backupMessageRect.bottom - pData->backupMessageRect.top));
        g.DrawLine(&backupInfoBorderPen, (REAL)pData->backupMessageRect.left, (REAL)pData->backupMessageRect.bottom - 1, (REAL)pData->backupMessageRect.right - 1, (REAL)pData->backupMessageRect.bottom - 1);
        g.DrawLine(&backupInfoBorderPen, (REAL)pData->backupMessageRect.left, (REAL)pData->backupMessageRect.top, (REAL)pData->backupMessageRect.left, (REAL)pData->backupMessageRect.bottom - 1);
        g.DrawLine(&backupInfoBorderPen, (REAL)pData->backupMessageRect.right - 1, (REAL)pData->backupMessageRect.top, (REAL)pData->backupMessageRect.right - 1, (REAL)pData->backupMessageRect.bottom - 1);

        Font backupInfoFont(hdc, pData->hFontSmall);
        SolidBrush backupInfoTextBrush(Color(255, 140, 140, 140));
        RectF backupInfoTextRect(12.0f, (REAL)pData->backupMessageRect.top, (REAL)(pData->backupMessageRect.right - 24), (REAL)(pData->backupMessageRect.bottom - pData->backupMessageRect.top));
        StringFormat sfBackupInfoText;
        sfBackupInfoText.SetAlignment(StringAlignmentCenter);
        sfBackupInfoText.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"Import or Export your current settings here.", -1, &backupInfoFont, backupInfoTextRect, &sfBackupInfoText, &backupInfoTextBrush);

        MainUI_Paint_DrawBackupButtonsRow(hdc, pData->importSettingsRect, pData->exportSettingsRect, pData->hFontText, pData->isHoveringImportSettings, pData->isHoveringExportSettings);

        if (g_updateFound.load() || g_updateCheckFailed.load()) {
            bool checkFailed = g_updateCheckFailed.load() && !g_updateFound.load();
            Color updateBannerBgColor = checkFailed ? (pData->isHoveringUpdateBanner ? Color(50, 80, 55, 50) : Color(30, 50, 40, 40)) : (pData->isHoveringUpdateBanner ? Color(50, 60, 80, 50) : Color(30, 40, 50, 40));
            SolidBrush updateBannerBgBrush(updateBannerBgColor);
            g.FillRectangle(&updateBannerBgBrush, (REAL)pData->updateBannerRect.left, (REAL)pData->updateBannerRect.top, (REAL)(pData->updateBannerRect.right - pData->updateBannerRect.left), (REAL)(pData->updateBannerRect.bottom - pData->updateBannerRect.top));
            Pen updateBannerSepPen(Color(100, 80, 80, 80), 1.0f);
            g.DrawLine(&updateBannerSepPen, (REAL)pData->updateBannerRect.left, (REAL)pData->updateBannerRect.top, (REAL)pData->updateBannerRect.right, (REAL)pData->updateBannerRect.top);

            HFONT updateBannerIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
            Font updateBannerIconFont(hdc, updateBannerIconFontH);
            SolidBrush updateBannerIconBrush(checkFailed ? Color(255, 210, 120, 80) : Color(255, 80, 200, 120));
            int updateBannerIconSize = 11;
            int updateBannerIconPadding = 2;
            int updateBannerIconX = pData->updateBannerRect.left + 20;
            int updateBannerIconY = pData->updateBannerRect.top + (pData->updateBannerRect.bottom - pData->updateBannerRect.top - updateBannerIconSize) / 2 - updateBannerIconPadding + 2;
            RectF updateBannerIconRect((REAL)updateBannerIconX, (REAL)updateBannerIconY, (REAL)(updateBannerIconSize + updateBannerIconPadding * 2), (REAL)(updateBannerIconSize + updateBannerIconPadding * 2));
            StringFormat sfUpdateBannerIcon;
            sfUpdateBannerIcon.SetAlignment(StringAlignmentCenter);
            sfUpdateBannerIcon.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(checkFailed ? L"\uEA39" : L"\uE896", -1, &updateBannerIconFont, updateBannerIconRect, &sfUpdateBannerIcon, &updateBannerIconBrush);
            DeleteObject(updateBannerIconFontH);

            Font updateBannerTextFont(hdc, pData->hFontText);
            SolidBrush updateBannerTextBrush(Color(255, 220, 220, 220));
            int updateBannerTextX = updateBannerIconX + updateBannerIconSize + updateBannerIconPadding * 2 + 10;
            RectF updateBannerTextRect((REAL)updateBannerTextX, (REAL)pData->updateBannerRect.top, (REAL)(pData->updateBannerRect.right - updateBannerTextX - 30), (REAL)32);
            StringFormat sfUpdateBannerText;
            sfUpdateBannerText.SetAlignment(StringAlignmentNear);
            sfUpdateBannerText.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(checkFailed ? L"Error to check updates" : L"Update Available", -1, &updateBannerTextFont, updateBannerTextRect, &sfUpdateBannerText, &updateBannerTextBrush);

            HFONT updateBannerArrowFontH = CreateFontW(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
            Font updateBannerArrowFont(hdc, updateBannerArrowFontH);
            SolidBrush updateBannerArrowBrush(Color(255, 150, 150, 150));
            int updateBannerArrowSize = 9;
            int updateBannerArrowPadding = 2;
            int updateBannerArrowX = pData->updateBannerRect.right - 20 - updateBannerArrowSize - updateBannerArrowPadding * 2;
            int updateBannerArrowY = pData->updateBannerRect.top + (pData->updateBannerRect.bottom - pData->updateBannerRect.top - updateBannerArrowSize) / 2 - updateBannerArrowPadding + 1;
            RectF updateBannerArrowRect((REAL)updateBannerArrowX, (REAL)updateBannerArrowY, (REAL)(updateBannerArrowSize + updateBannerArrowPadding * 2), (REAL)(updateBannerArrowSize + updateBannerArrowPadding * 2));
            StringFormat sfUpdateBannerArrow;
            sfUpdateBannerArrow.SetAlignment(StringAlignmentCenter);
            sfUpdateBannerArrow.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(L"\uE76C", -1, &updateBannerArrowFont, updateBannerArrowRect, &sfUpdateBannerArrow, &updateBannerArrowBrush);
            DeleteObject(updateBannerArrowFontH);
        }

        Color discordBgColor = pData->isHoveringDiscord ? Color(50, 60, 60, 60) : Color(30, 40, 40, 40);
        SolidBrush discordBgBrush(discordBgColor);
        g.FillRectangle(&discordBgBrush, (REAL)pData->discordButtonRect.left, (REAL)pData->discordButtonRect.top, (REAL)(pData->discordButtonRect.right - pData->discordButtonRect.left), (REAL)(pData->discordButtonRect.bottom - pData->discordButtonRect.top));
        Pen discordSepPen(Color(100, 80, 80, 80), 1.0f);
        g.DrawLine(&discordSepPen, (REAL)pData->discordButtonRect.left, (REAL)pData->discordButtonRect.top, (REAL)pData->discordButtonRect.right, (REAL)pData->discordButtonRect.top);

        HFONT discordIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font discordIconFont(hdc, discordIconFontH);
        SolidBrush discordIconBrush(Color(255, 88, 101, 242));
        int discordIconSize = 11;
        int discordIconPadding = 2;
        int discordIconX = pData->discordButtonRect.left + 20;
        int discordIconY = pData->discordButtonRect.top + (pData->discordButtonRect.bottom - pData->discordButtonRect.top - discordIconSize) / 2 - discordIconPadding + 2;
        RectF discordIconRect((REAL)discordIconX, (REAL)discordIconY, (REAL)(discordIconSize + discordIconPadding * 2), (REAL)(discordIconSize + discordIconPadding * 2));
        StringFormat sfDiscordIcon;
        sfDiscordIcon.SetAlignment(StringAlignmentCenter);
        sfDiscordIcon.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE8BD", -1, &discordIconFont, discordIconRect, &sfDiscordIcon, &discordIconBrush);
        DeleteObject(discordIconFontH);

        Font discordTextFont(hdc, pData->hFontText);
        SolidBrush discordTextBrush(Color(255, 220, 220, 220));
        int discordTextX = discordIconX + discordIconSize + discordIconPadding * 2 + 10;
        RectF discordTextRect((REAL)discordTextX, (REAL)pData->discordButtonRect.top, (REAL)(pData->discordButtonRect.right - discordTextX - 30), (REAL)32);
        StringFormat sfDiscordText;
        sfDiscordText.SetAlignment(StringAlignmentNear);
        sfDiscordText.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"Join our Discord", -1, &discordTextFont, discordTextRect, &sfDiscordText, &discordTextBrush);

        HFONT discordArrowFontH = CreateFontW(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font discordArrowFont(hdc, discordArrowFontH);
        SolidBrush discordArrowBrush(Color(255, 150, 150, 150));
        int discordArrowSize = 9;
        int discordArrowPadding = 2;
        int discordArrowX = pData->discordButtonRect.right - 20 - discordArrowSize - discordArrowPadding * 2;
        int discordArrowY = pData->discordButtonRect.top + (pData->discordButtonRect.bottom - pData->discordButtonRect.top - discordArrowSize) / 2 - discordArrowPadding + 1;
        RectF discordArrowRect((REAL)discordArrowX, (REAL)discordArrowY, (REAL)(discordArrowSize + discordArrowPadding * 2), (REAL)(discordArrowSize + discordArrowPadding * 2));
        StringFormat sfDiscordArrow;
        sfDiscordArrow.SetAlignment(StringAlignmentCenter);
        sfDiscordArrow.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE76C", -1, &discordArrowFont, discordArrowRect, &sfDiscordArrow, &discordArrowBrush);
        DeleteObject(discordArrowFontH);
    }
    else if (pData->currentPage == 1) { // Automation
        int rowCount = 9;
        int rowLeft = 0;
        int rowRight = clientRect.right;
        for (int i = 0; i < rowCount; i++) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rowLeft, (REAL)rr.top, (REAL)(rowRight - rowLeft), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rowLeft, (REAL)rr.bottom - 1, (REAL)rowRight, (REAL)rr.bottom - 1);
        }

        MainUI_Paint_DrawToggle(hdc, pData->autoStartToggleRect, pData->hFontText, L"Auto-Start AntiAFK", g_autoStartAfk.load(), PtInRect(&pData->autoStartToggleRect, pData->hoverPoint), pData->autoStartAnim, true, L"\uE768");
        MainUI_Paint_DrawToggle(hdc, pData->autoReconnectToggleRect, pData->hFontText, L"Auto Reconnect*", g_autoReconnect.load(), PtInRect(&pData->autoReconnectToggleRect, pData->hoverPoint), pData->autoReconnectAnim, true, L"\uE8AF");
        MainUI_Paint_DrawToggle(hdc, pData->autoResetToggleRect, pData->hFontText, L"Auto Reset (Esc+R+Enter)", g_autoReset.load(), PtInRect(&pData->autoResetToggleRect, pData->hoverPoint), pData->autoResetAnim, true, L"\uE81C");
        MainUI_Paint_DrawToggle(hdc, pData->autoHideToggleRect, pData->hFontText, L"Auto-Hide Roblox", g_autoHideRoblox.load(), PtInRect(&pData->autoHideToggleRect, pData->hoverPoint), pData->autoHideAnim, true, L"\uEE47");
        MainUI_Paint_DrawToggle(hdc, pData->autoOpacityToggleRect, pData->hFontText, L"Auto Opacity (70%)", g_autoOpacity.load(), PtInRect(&pData->autoOpacityToggleRect, pData->hoverPoint), pData->autoOpacityAnim, true, L"\uE727");
        MainUI_Paint_DrawToggle(hdc, pData->autoGridToggleRect, pData->hFontText, L"Auto Grid Roblox*", g_autoGrid.load(), PtInRect(&pData->autoGridToggleRect, pData->hoverPoint), pData->autoGridAnim, true, L"\uE80A");
        MainUI_Paint_DrawToggle(hdc, pData->bloxstrapIntegrationToggleRect, pData->hFontText, L"Fish/Void/Bloxstrap Integration", g_bloxstrapIntegration.load(), PtInRect(&pData->bloxstrapIntegrationToggleRect, pData->hoverPoint), pData->bloxstrapIntegrationAnim, true, L"\uE71B");
        MainUI_Paint_DrawToggle(hdc, pData->iCanForgetToggleRect, pData->hFontText, L"I can forget (smart auto-start)", g_afkReminderEnabled.load(), PtInRect(&pData->iCanForgetToggleRect, pData->hoverPoint), pData->iCanForgetAnim, true, L"\uE916");
        MainUI_Paint_DrawToggle(hdc, pData->doNotSleepToggleRect, pData->hFontText, L"Do not sleep", g_doNotSleep.load(), PtInRect(&pData->doNotSleepToggleRect, pData->hoverPoint), pData->doNotSleepAnim, true, L"\uE708");

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }
    }
    else if (pData->currentPage == 2) { // Statistics
        int rowCount = pData->rowRects.size();
        int rowLeft = 0;
        int rowRight = clientRect.right;
        for (int i = 0; i < rowCount; i++) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rowLeft, (REAL)rr.top, (REAL)(rowRight - rowLeft), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rowLeft, (REAL)rr.bottom - 1, (REAL)rowRight, (REAL)rr.bottom - 1);
        }

        HFONT statIconFontH = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font statIconFont(hdc, statIconFontH);
        SolidBrush statIconBrush(Color(255, 150, 150, 150));
        int statIconSize = 12;
        int statIconPadding = 2;

        auto drawStatIcon = [&](const RECT& rowRect, const wchar_t* icon, int verticalOffset = 0, int extraBoxSize = 0, int horizontalOffset = 0) {
            int iconBoxSize = statIconSize + statIconPadding * 2 + extraBoxSize;
            int iconY = rowRect.top + (rowRect.bottom - rowRect.top - iconBoxSize) / 2 + verticalOffset;
            RectF iconRectF((REAL)(20 + horizontalOffset), (REAL)iconY, (REAL)iconBoxSize, (REAL)iconBoxSize);
            StringFormat sfIcon;
            sfIcon.SetAlignment(StringAlignmentCenter);
            sfIcon.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(icon, -1, &statIconFont, iconRectF, &sfIcon, &statIconBrush);
        };

        drawStatIcon(pData->rowRects[0], L"\uE916");
        drawStatIcon(pData->rowRects[1], L"\uE8EE");
        drawStatIcon(pData->rowRects[2], L"\uE787");
        drawStatIcon(pData->rowRects[3], L"\uE7FC");
        drawStatIcon(pData->rowRects[4], L"\uE823", 1, 6, -2);
        drawStatIcon(pData->rowRects[5], L"\uE715");
        drawStatIcon(pData->rowRects[6], L"\uE7E8");
        drawStatIcon(pData->rowRects[7], L"\uE121");

        DeleteObject(statIconFontH);

        Font labelFont(hdc, pData->hFontText);
        SolidBrush labelBrush(Color(255, 220, 220, 220));
        StringFormat sfLabel;
        sfLabel.SetAlignment(StringAlignmentNear);
        sfLabel.SetLineAlignment(StringAlignmentCenter);
        int labelX = 20 + statIconSize + statIconPadding * 2 + 10;
        int labelWidth = 180;

        RectF lblRect0((REAL)labelX, (REAL)pData->rowRects[0].top, (REAL)labelWidth, (REAL)(pData->rowRects[0].bottom - pData->rowRects[0].top));
        g.DrawString(L"Total AFK Time", -1, &labelFont, lblRect0, &sfLabel, &labelBrush);
        RectF lblRect1((REAL)labelX, (REAL)pData->rowRects[1].top, (REAL)labelWidth, (REAL)(pData->rowRects[1].bottom - pData->rowRects[1].top));
        g.DrawString(L"Actions Performed", -1, &labelFont, lblRect1, &sfLabel, &labelBrush);
        RectF lblRect2((REAL)labelX, (REAL)pData->rowRects[2].top, (REAL)labelWidth, (REAL)(pData->rowRects[2].bottom - pData->rowRects[2].top));
        g.DrawString(L"Last Action", -1, &labelFont, lblRect2, &sfLabel, &labelBrush);
        RectF lblRect3((REAL)labelX, (REAL)pData->rowRects[3].top, (REAL)labelWidth, (REAL)(pData->rowRects[3].bottom - pData->rowRects[3].top));
        g.DrawString(L"Auto Reconnects", -1, &labelFont, lblRect3, &sfLabel, &labelBrush);
        RectF lblRect4((REAL)labelX, (REAL)pData->rowRects[4].top, (REAL)labelWidth, (REAL)(pData->rowRects[4].bottom - pData->rowRects[4].top));
        g.DrawString(L"Longest Session", -1, &labelFont, lblRect4, &sfLabel, &labelBrush);
        RectF lblRect5((REAL)labelX, (REAL)pData->rowRects[5].top, (REAL)labelWidth, (REAL)(pData->rowRects[5].bottom - pData->rowRects[5].top));
        g.DrawString(L"Webhooks Sent", -1, &labelFont, lblRect5, &sfLabel, &labelBrush);
        RectF lblRect6((REAL)labelX, (REAL)pData->rowRects[6].top, (REAL)labelWidth, (REAL)(pData->rowRects[6].bottom - pData->rowRects[6].top));
        g.DrawString(L"Program Launches", -1, &labelFont, lblRect6, &sfLabel, &labelBrush);
        RectF lblRect7((REAL)labelX, (REAL)pData->rowRects[7].top, (REAL)labelWidth, (REAL)(pData->rowRects[7].bottom - pData->rowRects[7].top));
        g.DrawString(L"AFK Sessions", -1, &labelFont, lblRect7, &sfLabel, &labelBrush);

        wchar_t buffer[128];
        Font valFont(hdc, pData->hFontText);
        SolidBrush valueBrush(Color(255, 255, 255, 255));
        StringFormat sfValue;
        sfValue.SetAlignment(StringAlignmentFar);
        sfValue.SetLineAlignment(StringAlignmentCenter);
        int valueRightMargin = 20;
        int valueWidth = 180;

        uint64_t total_seconds = g_totalAfkTimeSeconds.load();
        if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
            total_seconds += (GetTickCount64() - g_afkStartTime.load()) / 1000;
        }
        std::wstring totalAfkTimeText = FormatDurationShort(total_seconds);
        RectF valRect0((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[0].top, (REAL)valueWidth, (REAL)(pData->rowRects[0].bottom - pData->rowRects[0].top));
        g.DrawString(totalAfkTimeText.c_str(), -1, &valFont, valRect0, &sfValue, &valueBrush);

        swprintf_s(buffer, L"%llu", g_afkActionsPerformed.load());
        RectF valRect1((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[1].top, (REAL)valueWidth, (REAL)(pData->rowRects[1].bottom - pData->rowRects[1].top));
        g.DrawString(buffer, -1, &valFont, valRect1, &sfValue, &valueBrush);

        uint64_t lastActionTimestamp = g_lastAfkActionTimestamp.load();
        if (lastActionTimestamp > 0) {
            uint64_t elapsed = (GetTickCount64() - lastActionTimestamp) / 1000;
            if (elapsed < 60) swprintf_s(buffer, L"%llu seconds ago", elapsed);
            else if (elapsed < 3600) swprintf_s(buffer, L"%llu minutes ago", elapsed / 60);
            else swprintf_s(buffer, L"%llu hours ago", elapsed / 3600);
        } else {
            wcscpy_s(buffer, L"Never");
        }
        RectF valRect2((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[2].top, (REAL)valueWidth, (REAL)(pData->rowRects[2].bottom - pData->rowRects[2].top));
        g.DrawString(buffer, -1, &valFont, valRect2, &sfValue, &valueBrush);

        swprintf_s(buffer, L"%llu", g_autoReconnectsPerformed.load());
        RectF valRect3((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[3].top, (REAL)valueWidth, (REAL)(pData->rowRects[3].bottom - pData->rowRects[3].top));
        g.DrawString(buffer, -1, &valFont, valRect3, &sfValue, &valueBrush);

        std::wstring longestSessionText = FormatDurationShort(g_longestAfkSessionSeconds.load());
        RectF valRect4((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[4].top, (REAL)valueWidth, (REAL)(pData->rowRects[4].bottom - pData->rowRects[4].top));
        g.DrawString(longestSessionText.c_str(), -1, &valFont, valRect4, &sfValue, &valueBrush);

        swprintf_s(buffer, L"%llu", g_discordWebhooksSent.load());
        RectF valRect5((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[5].top, (REAL)valueWidth, (REAL)(pData->rowRects[5].bottom - pData->rowRects[5].top));
        g.DrawString(buffer, -1, &valFont, valRect5, &sfValue, &valueBrush);

        swprintf_s(buffer, L"%llu", g_programLaunches.load());
        RectF valRect6((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[6].top, (REAL)valueWidth, (REAL)(pData->rowRects[6].bottom - pData->rowRects[6].top));
        g.DrawString(buffer, -1, &valFont, valRect6, &sfValue, &valueBrush);

        swprintf_s(buffer, L"%llu", g_afkSessionsCompleted.load());
        RectF valRect7((REAL)(clientRect.right - valueWidth - valueRightMargin), (REAL)pData->rowRects[7].top, (REAL)valueWidth, (REAL)(pData->rowRects[7].bottom - pData->rowRects[7].top));
        g.DrawString(buffer, -1, &valFont, valRect7, &sfValue, &valueBrush);

        Color resetBgColor = pData->isHoveringResetStats ? Color(50, 60, 60, 60) : Color(30, 40, 40, 40);
        SolidBrush resetStatsBgBrush(resetBgColor);
        g.FillRectangle(&resetStatsBgBrush, (REAL)pData->resetStatsButtonRect.left, (REAL)pData->resetStatsButtonRect.top, (REAL)(pData->resetStatsButtonRect.right - pData->resetStatsButtonRect.left), (REAL)(pData->resetStatsButtonRect.bottom - pData->resetStatsButtonRect.top));

        Pen resetSepPen(Color(100, 80, 80, 80), 1.0f);
        g.DrawLine(&resetSepPen, (REAL)pData->resetStatsButtonRect.left, (REAL)pData->resetStatsButtonRect.top, (REAL)pData->resetStatsButtonRect.right, (REAL)pData->resetStatsButtonRect.top);

        HFONT resetIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font resetIconFont(hdc, resetIconFontH);
        SolidBrush resetIconBrush(Color(255, 200, 80, 80));
        int resetIconSize = 11;
        int resetIconPadding = 2;
        int resetIconX = pData->resetStatsButtonRect.left + 20;
        int resetIconY = pData->resetStatsButtonRect.top + (pData->resetStatsButtonRect.bottom - pData->resetStatsButtonRect.top - resetIconSize) / 2 - resetIconPadding + 1;
        RectF resetIconRect((REAL)resetIconX, (REAL)resetIconY, (REAL)(resetIconSize + resetIconPadding * 2), (REAL)(resetIconSize + resetIconPadding * 2));
        StringFormat sfResetIcon;
        sfResetIcon.SetAlignment(StringAlignmentCenter);
        sfResetIcon.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE74D", -1, &resetIconFont, resetIconRect, &sfResetIcon, &resetIconBrush);
        DeleteObject(resetIconFontH);

        Font resetTextFont(hdc, pData->hFontText);
        SolidBrush resetTextBrush(Color(255, 220, 220, 220));
        int resetTextX = resetIconX + resetIconSize + resetIconPadding * 2 + 10;
        int resetTextWidth = pData->resetStatsButtonRect.right - resetTextX - 20 - (9 + 4);
        RectF resetTextRect((REAL)resetTextX, (REAL)pData->resetStatsButtonRect.top, (REAL)resetTextWidth, (REAL)32);
        StringFormat sfResetText;
        sfResetText.SetAlignment(StringAlignmentNear);
        sfResetText.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"Reset Statistics", -1, &resetTextFont, resetTextRect, &sfResetText, &resetTextBrush);

        HFONT resetArrowFontH = CreateFontW(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font resetArrowFont(hdc, resetArrowFontH);
        SolidBrush resetArrowBrush(Color(255, 150, 150, 150));
        int resetArrowSize = 9;
        int resetArrowPadding = 2;
        int resetArrowX = pData->resetStatsButtonRect.right - 20 - resetArrowSize - resetArrowPadding * 2;
        int resetArrowY = pData->resetStatsButtonRect.top + (pData->resetStatsButtonRect.bottom - pData->resetStatsButtonRect.top - resetArrowSize) / 2 - resetArrowPadding + 1;
        RectF resetArrowRect((REAL)resetArrowX, (REAL)resetArrowY, (REAL)(resetArrowSize + resetArrowPadding * 2), (REAL)(resetArrowSize + resetArrowPadding * 2));
        StringFormat sfResetArrow;
        sfResetArrow.SetAlignment(StringAlignmentCenter);
        sfResetArrow.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE76C", -1, &resetArrowFont, resetArrowRect, &sfResetArrow, &resetArrowBrush);
        DeleteObject(resetArrowFontH);

    }
    else if (pData->currentPage == 3) { // Advanced
        int rowCount = pData->rowRects.size();
        int rowLeft = 0;
        int rowRight = clientRect.right;
        for (int i = 0; i < rowCount; i++) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rowLeft, (REAL)rr.top, (REAL)(rowRight - rowLeft), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rowLeft, (REAL)rr.bottom - 1, (REAL)rowRight, (REAL)rr.bottom - 1);
        }

        MainUI_Paint_DrawToggle(hdc, pData->fishstrapToggleRect, pData->hFontText, L"FishStrap/Shaders Support", g_fishstrapSupport.load(), PtInRect(&pData->fishstrapToggleRect, pData->hoverPoint), pData->fishstrapAnim, true, L"\uE790");
        MainUI_Paint_DrawToggle(hdc, pData->autoUpdateToggleRect, pData->hFontText, L"Update Checker", g_autoUpdate.load(), PtInRect(&pData->autoUpdateToggleRect, pData->hoverPoint), pData->autoUpdateAnim, true, L"\uE777");
        MainUI_Paint_DrawToggle(hdc, pData->legacyUiToggleRect, pData->hFontText, L"Use Legacy UI (Tray)", g_useLegacyUi.load(), PtInRect(&pData->legacyUiToggleRect, pData->hoverPoint), pData->legacyUiAnim, true, L"\uE81C");
        MainUI_Paint_DrawToggle(hdc, pData->statusBarToggleRect, pData->hFontText, L"Status Bar*", g_statusBarEnabled.load(), PtInRect(&pData->statusBarToggleRect, pData->hoverPoint), pData->statusBarAnim, true, L"\uE7F4");

            wchar_t fpsLabel[48];
            if (g_fpsLimit == 0) swprintf_s(fpsLabel, L"Off");
            else swprintf_s(fpsLabel, L"%d FPS", g_fpsLimit);
            MainUI_Paint_DrawDropdown(hdc, pData->fpsCapperDropdownRect, pData->hFontText, L"FPS Capper*", fpsLabel, pData->isHoveringFpsCapper, true, L"\uF272");
        MainUI_Paint_DrawToggle(hdc, pData->unlockFpsOnFocusToggleRect, pData->hFontText, L"Unlock FPS when focus", g_unlockFpsOnFocus.load(), PtInRect(&pData->unlockFpsOnFocusToggleRect, pData->hoverPoint), pData->unlockFpsOnFocusAnim, true, L"\uF272");
        MainUI_Paint_DrawToggle(hdc, pData->autoMuteToggleRect, pData->hFontText, L"AutoMute Roblox", g_autoMute.load(), PtInRect(&pData->autoMuteToggleRect, pData->hoverPoint), pData->autoMuteAnim, true, L"\uE767");
        MainUI_Paint_DrawToggle(hdc, pData->unmuteOnFocusToggleRect, pData->hFontText, L"Unmute on focus", g_unmuteOnFocus.load(), PtInRect(&pData->unmuteOnFocusToggleRect, pData->hoverPoint), pData->unmuteOnFocusAnim, true, L"\uE767");
        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }

        Color resetBgColor = pData->isHoveringReset ? Color(50, 60, 60, 60) : Color(30, 40, 40, 40);
        SolidBrush resetBrush(resetBgColor);
        g.FillRectangle(&resetBrush, (REAL)pData->resetSettingsButtonRect.left, (REAL)pData->resetSettingsButtonRect.top, (REAL)(pData->resetSettingsButtonRect.right - pData->resetSettingsButtonRect.left), (REAL)(pData->resetSettingsButtonRect.bottom - pData->resetSettingsButtonRect.top));
        Pen resetSepPen(Color(100, 80, 80, 80), 1.0f);
        g.DrawLine(&resetSepPen, (REAL)pData->resetSettingsButtonRect.left, (REAL)pData->resetSettingsButtonRect.top, (REAL)pData->resetSettingsButtonRect.right, (REAL)pData->resetSettingsButtonRect.top);

        HFONT resetIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font resetIconFont(hdc, resetIconFontH);
        SolidBrush resetIconBrush(Color(255, 200, 80, 80));
        int resetIconSize2 = 11;
        int resetIconPadding2 = 2;
        int resetIconX2 = pData->resetSettingsButtonRect.left + 20;
        int resetIconY2 = pData->resetSettingsButtonRect.top + (pData->resetSettingsButtonRect.bottom - pData->resetSettingsButtonRect.top - resetIconSize2) / 2 - resetIconPadding2 + 1;
        RectF resetIconRect2((REAL)resetIconX2, (REAL)resetIconY2, (REAL)(resetIconSize2 + resetIconPadding2 * 2), (REAL)(resetIconSize2 + resetIconPadding2 * 2));
        StringFormat sfResetIcon2;
        sfResetIcon2.SetAlignment(StringAlignmentCenter);
        sfResetIcon2.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE74D", -1, &resetIconFont, resetIconRect2, &sfResetIcon2, &resetIconBrush);
        DeleteObject(resetIconFontH);

        Font resetTextFont(hdc, pData->hFontText);
        SolidBrush resetTextBrush(Color(255, 220, 220, 220));
        int resetTextX2 = resetIconX2 + resetIconSize2 + resetIconPadding2 * 2 + 10;
        int resetTextWidth2 = pData->resetSettingsButtonRect.right - resetTextX2 - 20 - (8 + 4);
        RectF resetTextRect2((REAL)resetTextX2, (REAL)pData->resetSettingsButtonRect.top, (REAL)resetTextWidth2, (REAL)32);
        StringFormat sfResetText2;
        sfResetText2.SetAlignment(StringAlignmentNear);
        sfResetText2.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"Reset All Settings", -1, &resetTextFont, resetTextRect2, &sfResetText2, &resetTextBrush);

        HFONT resetArrowFontH = CreateFontW(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font resetArrowFont(hdc, resetArrowFontH);
        SolidBrush resetArrowBrush(Color(255, 150, 150, 150));
        int resetArrowSize = 9;
        int resetArrowPadding = 2;
        int resetArrowX = pData->resetSettingsButtonRect.right - 20 - resetArrowSize - resetArrowPadding * 2;
        int resetArrowY = pData->resetSettingsButtonRect.top + (pData->resetSettingsButtonRect.bottom - pData->resetSettingsButtonRect.top - resetArrowSize) / 2 - resetArrowPadding + 1;
        RectF resetArrowRect((REAL)resetArrowX, (REAL)resetArrowY, (REAL)(resetArrowSize + resetArrowPadding * 2), (REAL)(resetArrowSize + resetArrowPadding * 2));
        StringFormat sfResetArrow;
        sfResetArrow.SetAlignment(StringAlignmentCenter);
        sfResetArrow.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE76C", -1, &resetArrowFont, resetArrowRect, &sfResetArrow, &resetArrowBrush);
        DeleteObject(resetArrowFontH);
    }
    else if (pData->currentPage == 5) { // Discord
        int rowCount = (int)pData->rowRects.size();
        for (int i = 0; i < rowCount; ++i) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rr.left, (REAL)rr.top, (REAL)(rr.right - rr.left), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rr.left, (REAL)rr.bottom - 1, (REAL)rr.right, (REAL)rr.bottom - 1);
        }

        SolidBrush infoBgBrush(Color(60, 45, 45, 45));
        Pen infoBorderPen(Color(180, 80, 80, 80), 1.0f);
        g.FillRectangle(&infoBgBrush, (REAL)pData->discordPayloadInfoRect.left, (REAL)pData->discordPayloadInfoRect.top, (REAL)(pData->discordPayloadInfoRect.right - pData->discordPayloadInfoRect.left), (REAL)(pData->discordPayloadInfoRect.bottom - pData->discordPayloadInfoRect.top));
        g.DrawRectangle(&infoBorderPen, (REAL)pData->discordPayloadInfoRect.left, (REAL)pData->discordPayloadInfoRect.top, (REAL)(pData->discordPayloadInfoRect.right - pData->discordPayloadInfoRect.left - 1), (REAL)(pData->discordPayloadInfoRect.bottom - pData->discordPayloadInfoRect.top - 1));

        HFONT discordInfoIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
        Font discordInfoIconFont(hdc, discordInfoIconFontH);
        SolidBrush discordInfoIconBrush(Color(255, 88, 101, 242));
        RectF discordInfoIconRect((REAL)12, (REAL)pData->discordPayloadInfoRect.top, 20.0f, (REAL)(pData->discordPayloadInfoRect.bottom - pData->discordPayloadInfoRect.top));
        StringFormat sfDiscordInfoIcon;
        sfDiscordInfoIcon.SetAlignment(StringAlignmentCenter);
        sfDiscordInfoIcon.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\uE8BD", -1, &discordInfoIconFont, discordInfoIconRect, &sfDiscordInfoIcon, &discordInfoIconBrush);
        DeleteObject(discordInfoIconFontH);

        Font discordInfoFont(hdc, pData->hFontSmall);
        SolidBrush discordInfoTextBrush(Color(255, 140, 140, 140));
        RectF discordInfoTextRect(36.0f, (REAL)pData->discordPayloadInfoRect.top, (REAL)(pData->discordPayloadInfoRect.right - 48), (REAL)(pData->discordPayloadInfoRect.bottom - pData->discordPayloadInfoRect.top));
        StringFormat sfDiscordInfoText;
        sfDiscordInfoText.SetAlignment(StringAlignmentNear);
        sfDiscordInfoText.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"Discord webhook posts short status updates.", -1, &discordInfoFont, discordInfoTextRect, &sfDiscordInfoText, &discordInfoTextBrush);

        std::wstring webhookUrl = GetDiscordWebhookUrlCopy();
        MainUI_Paint_DrawToggle(hdc, pData->discordWebhookEnableToggleRect, pData->hFontText, L"Enable webhook", g_discordWebhookEnabled.load(), PtInRect(&pData->discordWebhookEnableToggleRect, pData->hoverPoint), pData->discordWebhookEnableAnim, true, L"\uE8BD");
        MainUI_Paint_DrawTextInput(hdc, pData->discordWebhookInputRect, pData->hFontSmall, pData->hFontText, L"", webhookUrl, pData->isHoveringDiscordWebhookInput, pData->isDiscordWebhookInputFocused, &pData->discordWebhookCaretPos, &pData->discordWebhookViewOffset, L"https://discord.com/api/webhooks/...");

        MainUI_Paint_DrawTripleActionButtonsRow(
            hdc,
            pData->discordWebhookPasteRect,
            pData->discordWebhookClearRect,
            pData->discordWebhookTestRect,
            pData->hFontText,
            L"Paste",
            L"Clear",
            L"Send Test",
            pData->isHoveringDiscordWebhookPaste,
            pData->isHoveringDiscordWebhookClear,
            pData->isHoveringDiscordWebhookTest,
            true);

        MainUI_Paint_DrawToggle(hdc, pData->discordNotifyStartToggleRect, pData->hFontText, L"Start / Stop", g_discordNotifyStart.load(), PtInRect(&pData->discordNotifyStartToggleRect, pData->hoverPoint), pData->discordNotifyStartAnim, true, L"\uE768");
        MainUI_Paint_DrawToggle(hdc, pData->discordNotifyActionToggleRect, pData->hFontText, L"Action", g_discordNotifyAction.load(), PtInRect(&pData->discordNotifyActionToggleRect, pData->hoverPoint), pData->discordNotifyActionAnim, true, L"\uE7C9");
        MainUI_Paint_DrawToggle(hdc, pData->discordNotifyReconnectToggleRect, pData->hFontText, L"Reconnect", g_discordNotifyReconnect.load(), PtInRect(&pData->discordNotifyReconnectToggleRect, pData->hoverPoint), pData->discordNotifyReconnectAnim, true, L"\uE8AF");
        MainUI_Paint_DrawToggle(hdc, pData->discordNotifyErrorsToggleRect, pData->hFontText, L"Errors", g_discordNotifyErrors.load(), PtInRect(&pData->discordNotifyErrorsToggleRect, pData->hoverPoint), pData->discordNotifyErrorsAnim, true, L"\uEA39");
        MainUI_Paint_DrawToggle(hdc, pData->discordDisableEmbedToggleRect, pData->hFontText, L"Disable embed", g_discordDisableEmbed.load(), PtInRect(&pData->discordDisableEmbedToggleRect, pData->hoverPoint), pData->discordDisableEmbedAnim, true, L"\uE8FD");
        MainUI_Paint_DrawToggle(hdc, pData->discordMentionOnErrorsToggleRect, pData->hFontText, L"Mention on errors", g_discordMentionOnErrors.load(), PtInRect(&pData->discordMentionOnErrorsToggleRect, pData->hoverPoint), pData->discordMentionOnErrorsAnim, true, L"\uE7BA");

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }
    }
    else if (pData->currentPage == 4) { // Utils
        int rowCount = (int)pData->rowRects.size();
        for (int i = 0; i < rowCount; ++i) {
            RECT rr = pData->rowRects[i];
            Color rowBgColor = (i % 2 == 0) ? Color(30, 35, 35, 35) : Color(50, 50, 50, 50);
            SolidBrush rowBrush(rowBgColor);
            g.FillRectangle(&rowBrush, (REAL)rr.left, (REAL)rr.top, (REAL)(rr.right - rr.left), (REAL)(rr.bottom - rr.top));
            Pen sepPen(Color(60, 70, 70, 70), 1.0f);
            g.DrawLine(&sepPen, (REAL)rr.left, (REAL)rr.bottom - 1, (REAL)rr.right, (REAL)rr.bottom - 1);
        }

        RECT utilsWindowOpacityDrawRect = { 20, pData->utilsWindowOpacityRect.top, pData->utilsWindowOpacityRect.right, pData->utilsWindowOpacityRect.top + 32 };
        RECT utilsMuteDrawRect = { 20, pData->utilsMuteToggleRect.top, pData->utilsMuteToggleRect.right, pData->utilsMuteToggleRect.top + 32 };
        RECT utilsFpsDrawRect = { 20, pData->utilsFpsToggleRect.top, pData->utilsFpsToggleRect.right, pData->utilsFpsToggleRect.top + 32 };

        MainUI_Paint_DrawListActionRow(hdc, pData->utilsShowRobloxRect, pData->hFontText, L"Show Roblox", pData->isHoveringUtilsShowRoblox, L"\uE890");
        MainUI_Paint_DrawListActionRow(hdc, pData->utilsHideRobloxRect, pData->hFontText, L"Hide Roblox", pData->isHoveringUtilsHideRoblox, L"\uE890\uEA83");
        MainUI_Paint_DrawListActionRow(hdc, pData->utilsGridSnapRect, pData->hFontText, L"Grid Snap Roblox*", pData->isHoveringUtilsGridSnap, L"\uE80A");
        MainUI_Paint_DrawToggle(hdc, utilsWindowOpacityDrawRect, pData->hFontText, L"Window Opacity", IsUtilsSessionWindowOpacityEnabled(), PtInRect(&pData->utilsWindowOpacityRect, pData->hoverPoint), pData->utilsWindowOpacityAnim, true, L"\uE727");
        MainUI_Paint_DrawListActionRow(hdc, pData->utilsCloseAllRect, pData->hFontText, L"Close All Roblox", pData->isHoveringUtilsCloseAll, L"\uE8BB");
        MainUI_Paint_DrawListActionRow(hdc, pData->utilsResetAllRect, pData->hFontText, L"Reset in all", pData->isHoveringUtilsResetAll, L"\uE81C");
        MainUI_Paint_DrawListActionRow(hdc, pData->utilsTestActionRect, pData->hFontText, L"Test AntiAFK action", pData->isHoveringUtilsTestAction, L"\uE768");
        MainUI_Paint_DrawToggle(hdc, utilsMuteDrawRect, pData->hFontText, L"Mute/UnMute Roblox", IsUtilsSessionMuteEnabled(), PtInRect(&pData->utilsMuteToggleRect, pData->hoverPoint), pData->utilsMuteAnim, true, L"\uE767");
        MainUI_Paint_DrawToggle(hdc, utilsFpsDrawRect, pData->hFontText, L"Limit/Unlimit FPS*", IsUtilsSessionFpsLimitEnabled(), PtInRect(&pData->utilsFpsToggleRect, pData->hoverPoint), pData->utilsFpsAnim, true, L"\uF272");

        for (size_t i = 0; i < pData->helpButtonRects.size(); ++i) {
            MainUI_Paint_DrawHelpButton(hdc, pData->helpButtonRects[i], pData->hFontText, pData->hoveringHelpButton == i);
        }
    }
    
    SelectObject(hdc, oldFont);

    const wchar_t* disclaimerText;
    if (pData->currentPage == 2) {
        disclaimerText = L"Statistics are saved only when the app is closed correctly.";
    } else if (pData->currentPage == 3) {
        disclaimerText = L"* - experimental feature.";
    } else if (pData->currentPage == 4) {
        disclaimerText = L"Utils changes apply now and reset on exit.";
    } else if (pData->currentPage == 5) {
        disclaimerText = L"Webhook URL is stored locally in your Windows user profile.";
    } else {
        disclaimerText = L"Closing this window minimizes it to the system tray.";
    }

    HFONT discIconFontH = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe MDL2 Assets");
    Font discIconFont(hdc, discIconFontH);
    SolidBrush discIconBrush(Color(255, 150, 150, 150));
    int discIconSize = 10;
    int discIconGap = 5;
    int discIconPadding = 4;

    Font discFont(hdc, pData->hFontSmall);
    SizeF availableSize((REAL)(pData->disclaimerRect.right - pData->disclaimerRect.left - 40), (REAL)(pData->disclaimerRect.bottom - pData->disclaimerRect.top));
    RectF discTextBounds;
    g.MeasureString(disclaimerText, -1, &discFont, PointF(0, 0), StringFormat::GenericTypographic(), &discTextBounds);
    int textWidth = (int)discTextBounds.Width;

    int discContentWidth = discIconSize + discIconGap + textWidth;
    int centerX = (pData->disclaimerRect.left + pData->disclaimerRect.right) / 2;
    int discStartX = centerX - discContentWidth / 2;
    int discIconX = discStartX - discIconPadding;
    int discIconY = pData->disclaimerRect.top + (pData->disclaimerRect.bottom - pData->disclaimerRect.top - discIconSize) / 2 - discIconPadding;
    RectF discIconRectF((REAL)discIconX, (REAL)discIconY, (REAL)(discIconSize + discIconPadding * 2), (REAL)(discIconSize + discIconPadding * 2));
    StringFormat sfDiscIcon;
    sfDiscIcon.SetAlignment(StringAlignmentCenter);
    sfDiscIcon.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(L"\uE946", -1, &discIconFont, discIconRectF, &sfDiscIcon, &discIconBrush);
    DeleteObject(discIconFontH);

    SolidBrush disclaimerBgBrush(Color(60, 45, 45, 45));
    g.FillRectangle(&disclaimerBgBrush, (REAL)pData->disclaimerRect.left, (REAL)pData->disclaimerRect.top, (REAL)(pData->disclaimerRect.right - pData->disclaimerRect.left), (REAL)(pData->disclaimerRect.bottom - pData->disclaimerRect.top));

    Pen disclaimerBorderPen(Color(180, 80, 80, 80), 1.0f);
    g.DrawRectangle(&disclaimerBorderPen, (REAL)pData->disclaimerRect.left, (REAL)pData->disclaimerRect.top, (REAL)(pData->disclaimerRect.right - pData->disclaimerRect.left - 1), (REAL)(pData->disclaimerRect.bottom - pData->disclaimerRect.top - 1));

    SolidBrush disclaimerBrush(Color(255, 140, 140, 140));
    int discTextX = discStartX + discIconSize + discIconGap;
    int discTextWidth = pData->disclaimerRect.right - discTextX;
    RectF disclaimerRectF((REAL)discTextX, (REAL)pData->disclaimerRect.top, (REAL)discTextWidth, (REAL)(pData->disclaimerRect.bottom - pData->disclaimerRect.top));
    StringFormat sfDisclaimer;
    sfDisclaimer.SetAlignment(StringAlignmentNear);
    sfDisclaimer.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(disclaimerText, -1, &discFont, disclaimerRectF, &sfDisclaimer, &disclaimerBrush);
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
        pData->hFontNavIcon = CreateFontW(-MulDiv(12, dpiY, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe MDL2 Assets");

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        const int startBtnH = 40;
        const int disclaimerH = 26;
        pData->startButtonRect = { 0, clientRect.bottom - startBtnH, clientRect.right, clientRect.bottom };

        PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(clientRect.right, clientRect.bottom));
        pData->navItems.push_back({ L"General", L"\xE80F", {0, 0, 0, 0}, 0.0f });
        pData->navItems.push_back({ L"Auto", L"\xE945", {0, 0, 0, 0}, 0.0f });
        pData->navItems.push_back({ L"Statistics", L"\xE9D2", {0, 0, 0, 0}, 0.0f });
        pData->navItems.push_back({ L"Advanced", L"\xE713", {0, 0, 0, 0}, 0.0f });
        pData->navItems.push_back({ L"Utils", L"\xE7FC", {0, 0, 0, 0}, 0.0f });
        pData->navItems.push_back({ L"Webhook", L"\xE8BD", {0, 0, 0, 0}, 0.0f });


        pData->hCursorHand = LoadCursor(NULL, IDC_HAND);
        pData->hCursorArrow = LoadCursor(NULL, IDC_ARROW);
        pData->hCursorText = LoadCursor(NULL, IDC_IBEAM);

        pData->hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_OFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

        pData->multiInstanceAnim = g_multiSupport.load() ? 1.0f : 0.0f;
        pData->fishstrapAnim = g_fishstrapSupport.load() ? 1.0f : 0.0f;
        pData->autoUpdateAnim = g_autoUpdate.load() ? 1.0f : 0.0f;
        pData->autoStartAnim = g_autoStartAfk.load() ? 1.0f : 0.0f;
        pData->autoReconnectAnim = g_autoReconnect.load() ? 1.0f : 0.0f;
        pData->autoResetAnim = g_autoReset.load() ? 1.0f : 0.0f;
        pData->autoHideAnim = g_autoHideRoblox.load() ? 1.0f : 0.0f;
        pData->autoOpacityAnim = g_autoOpacity.load() ? 1.0f : 0.0f;
        pData->autoGridAnim = g_autoGrid.load() ? 1.0f : 0.0f;
        pData->bloxstrapIntegrationAnim = g_bloxstrapIntegration.load() ? 1.0f : 0.0f;
        pData->unlockFpsOnFocusAnim = g_unlockFpsOnFocus.load() ? 1.0f : 0.0f;

        pData->previousAfkStartedState = g_isAfkStarted.load();
        pData->stateChangeProgress = 1.0f;

        pData->legacyUiAnim = g_useLegacyUi.load() ? 1.0f : 0.0f;
        pData->statusBarAnim = g_statusBarEnabled.load() ? 1.0f : 0.0f;
        pData->iCanForgetAnim = g_afkReminderEnabled.load() ? 1.0f : 0.0f;
        pData->doNotSleepAnim = g_doNotSleep.load() ? 1.0f : 0.0f;
        pData->autoMuteAnim = g_autoMute.load() ? 1.0f : 0.0f;
        pData->unmuteOnFocusAnim = g_unmuteOnFocus.load() ? 1.0f : 0.0f;
        pData->utilsWindowOpacityAnim = IsUtilsSessionWindowOpacityEnabled() ? 1.0f : 0.0f;
        pData->utilsMuteAnim = IsUtilsSessionMuteEnabled() ? 1.0f : 0.0f;
        pData->utilsFpsAnim = IsUtilsSessionFpsLimitEnabled() ? 1.0f : 0.0f;
        pData->discordWebhookEnableAnim = g_discordWebhookEnabled.load() ? 1.0f : 0.0f;
        pData->discordNotifyStartAnim = g_discordNotifyStart.load() ? 1.0f : 0.0f;
        pData->discordNotifyStopAnim = g_discordNotifyStop.load() ? 1.0f : 0.0f;
        pData->discordNotifyActionAnim = g_discordNotifyAction.load() ? 1.0f : 0.0f;
        pData->discordNotifyReconnectAnim = g_discordNotifyReconnect.load() ? 1.0f : 0.0f;
        pData->discordNotifyResetAnim = g_discordNotifyReset.load() ? 1.0f : 0.0f;
        pData->discordNotifyErrorsAnim = g_discordNotifyErrors.load() ? 1.0f : 0.0f;
        pData->discordDisableEmbedAnim = g_discordDisableEmbed.load() ? 1.0f : 0.0f;
        pData->discordMentionOnErrorsAnim = g_discordMentionOnErrors.load() ? 1.0f : 0.0f;

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
        const int margin = 20, rowH = 32, vGap = 10, toggleW = 50;
        const int nav_bar_y = 30, nav_bar_h = 30;
        const int startBtnH = 40;
        const int content_y_start = nav_bar_y + nav_bar_h;
        int y = content_y_start;

        pData->startButtonRect = { 0, clientRect.bottom - startBtnH, clientRect.right, clientRect.bottom };

        int help_btn_size = 24;
        int ctrlStartX = margin;
        int ctrlEndX = clientRect.right - 20;

        const int disclaimerH = 26;

        pData->helpButtonRects.clear();
        pData->rowRects.clear();
        if (pData->currentPage == 0) {
            int ddW_interval = 166;
            int ddW_action = 144;
            int ddW_mi_interval = 143;
            int ddW_user_safe = 181;
            int ddW_restore = 161;
            int ddW_default = ctrlEndX - ctrlStartX - help_btn_size;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->intervalDropdownRect = { ctrlEndX - ddW_interval - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->actionDropdownRect = { ctrlEndX - ddW_action - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->multiInstanceToggleRect = { ctrlStartX, y, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->multiInstanceIntervalDropdownRect = { ctrlEndX - ddW_mi_interval - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->userSafeDropdownRect = { ctrlEndX - ddW_user_safe - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->restoreMethodDropdownRect = { ctrlEndX - ddW_restore - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->backupMessageRect = { 0, y, clientRect.right, y + 26 };
            y += 25;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            int halfX = clientRect.right / 2;
            pData->importSettingsRect = { 0, y, halfX, y + rowH };
            pData->exportSettingsRect = { halfX, y, clientRect.right, y + rowH };
            y += rowH + vGap;

            pData->discordButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
            pData->updateBannerRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH * 2, clientRect.right, clientRect.bottom - startBtnH - disclaimerH - rowH };
        } else if (pData->currentPage == 1) { // Automation
            int toggleW = ctrlEndX - ctrlStartX - help_btn_size;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoStartToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoReconnectToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoResetToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoHideToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoOpacityToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoGridToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->bloxstrapIntegrationToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->iCanForgetToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->doNotSleepToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;
        } else if (pData->currentPage == 2) { // Statistics
            int toggleW = ctrlEndX - ctrlStartX - help_btn_size;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->resetStatsButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
        } else if (pData->currentPage == 3) { // Advanced
            int ddW_fps = 173;
            int ctrlW = ctrlEndX - ctrlStartX - help_btn_size;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->fishstrapToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoUpdateToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->legacyUiToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->statusBarToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

        pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
        pData->fpsCapperDropdownRect = { ctrlEndX - ddW_fps - help_btn_size, y + 8, ctrlEndX - help_btn_size, y + rowH };
        pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->unlockFpsOnFocusToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->autoMuteToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->unmuteOnFocusToggleRect = { ctrlStartX, y, ctrlStartX + ctrlW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->resetSettingsButtonRect = { 0, clientRect.bottom - startBtnH - disclaimerH - rowH, clientRect.right, clientRect.bottom - startBtnH - disclaimerH };
        } else if (pData->currentPage == 5) { // Discord
            const int noteH = 26;
            const int buttonGap = 0;
            int toggleW = ctrlEndX - ctrlStartX - help_btn_size;
            int buttonW = clientRect.right / 3;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordWebhookEnableToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->discordPayloadInfoRect = { 0, y, clientRect.right, y + noteH };
            y += noteH - 1;

            pData->discordWebhookInputRect = { 0, y, clientRect.right, y + rowH };
            y += rowH - 1;

            pData->discordWebhookPasteRect = { 0, y, buttonW, y + rowH };
            pData->discordWebhookClearRect = { pData->discordWebhookPasteRect.right + buttonGap, y, pData->discordWebhookPasteRect.right + buttonGap + buttonW, y + rowH };
            pData->discordWebhookTestRect = { pData->discordWebhookClearRect.right + buttonGap, y, clientRect.right, y + rowH };
            y += rowH;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordNotifyStartToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordNotifyActionToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordNotifyReconnectToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordNotifyErrorsToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordDisableEmbedToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
            y += rowH + vGap;

            pData->rowRects.push_back({ 0, y, clientRect.right, y + rowH + vGap });
            pData->discordMentionOnErrorsToggleRect = { ctrlStartX, y, ctrlStartX + toggleW, y + rowH };
            pData->helpButtonRects.push_back({ ctrlEndX - help_btn_size, y + (rowH - help_btn_size) / 2 + 4, ctrlEndX, y + (rowH - help_btn_size) / 2 + help_btn_size + 4 });
        }
        
        pData->disclaimerRect = { 0, clientRect.bottom - startBtnH - disclaimerH + 1, clientRect.right, clientRect.bottom - startBtnH + 1 };
        
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }
    case WM_KILLFOCUS:
        if (pData && pData->isDiscordWebhookInputFocused) {
            pData->isDiscordWebhookInputFocused = false;
            InvalidateRect(hwnd, &pData->discordWebhookInputRect, FALSE);
        }
        break;
    case WM_KEYDOWN:
        if (pData && pData->currentPage == 5 && pData->isDiscordWebhookInputFocused) {
            std::wstring currentValue = GetDiscordWebhookUrlCopy();
            size_t* caretPos = &pData->discordWebhookCaretPos;
            RECT* inputRect = &pData->discordWebhookInputRect;
            size_t maxLength = DISCORD_WEBHOOK_MAX_LENGTH;
            *caretPos = min(*caretPos, currentValue.size());

            if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'V') {
                std::wstring clipboardText;
                if (ReadUnicodeTextFromClipboard(hwnd, clipboardText)) {
                    if (!clipboardText.empty()) {
                        size_t insertCount = min(clipboardText.size(), maxLength - min(currentValue.size(), maxLength));
                        if (insertCount > 0) {
                            currentValue.insert(*caretPos, clipboardText.substr(0, insertCount));
                            *caretPos += insertCount;
                            SetDiscordWebhookUrl(currentValue);
                            *caretPos = min(*caretPos, GetDiscordWebhookUrlCopy().size());
                            SaveSettings();
                            InvalidateRect(hwnd, inputRect, FALSE);
                        }
                    }
                }
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                pData->isDiscordWebhookInputFocused = false;
                InvalidateRect(hwnd, inputRect, FALSE);
                return 0;
            }
            if (wParam == VK_LEFT) {
                if (*caretPos > 0) {
                    --(*caretPos);
                    InvalidateRect(hwnd, inputRect, FALSE);
                }
                return 0;
            }
            if (wParam == VK_RIGHT) {
                if (*caretPos < currentValue.size()) {
                    ++(*caretPos);
                    InvalidateRect(hwnd, inputRect, FALSE);
                }
                return 0;
            }
            if (wParam == VK_HOME) {
                *caretPos = 0;
                InvalidateRect(hwnd, inputRect, FALSE);
                return 0;
            }
            if (wParam == VK_END) {
                *caretPos = currentValue.size();
                InvalidateRect(hwnd, inputRect, FALSE);
                return 0;
            }
            if (wParam == VK_DELETE) {
                if (*caretPos < currentValue.size()) {
                    currentValue.erase(*caretPos, 1);
                    SetDiscordWebhookUrl(currentValue);
                    *caretPos = min(*caretPos, GetDiscordWebhookUrlCopy().size());
                    SaveSettings();
                    InvalidateRect(hwnd, inputRect, FALSE);
                }
                return 0;
            }
        }
        break;
    case WM_CHAR:
        if (pData && pData->currentPage == 5 && pData->isDiscordWebhookInputFocused) {
            std::wstring currentValue = GetDiscordWebhookUrlCopy();
            size_t* caretPos = &pData->discordWebhookCaretPos;
            RECT* inputRect = &pData->discordWebhookInputRect;
            size_t maxLength = DISCORD_WEBHOOK_MAX_LENGTH;
            *caretPos = min(*caretPos, currentValue.size());
            bool changed = false;

            if (wParam == VK_BACK) {
                if (*caretPos > 0 && !currentValue.empty()) {
                    currentValue.erase(*caretPos - 1, 1);
                    --(*caretPos);
                    changed = true;
                }
            }
            else if (wParam >= 0x20 && !iswcntrl((wchar_t)wParam) && !iswspace((wchar_t)wParam)) {
                if (currentValue.size() < maxLength) {
                    currentValue.insert(currentValue.begin() + (ptrdiff_t)(*caretPos), (wchar_t)wParam);
                    ++(*caretPos);
                    changed = true;
                }
            }

            if (changed) {
                SetDiscordWebhookUrl(currentValue);
                *caretPos = min(*caretPos, GetDiscordWebhookUrlCopy().size());
                SaveSettings();
                InvalidateRect(hwnd, inputRect, FALSE);
            }
            return 0;
        }
        break;

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
        bool anyTextHover = false;
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
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringMultiInstanceInterval, pData->multiInstanceIntervalDropdownRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringDiscord, pData->discordButtonRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringUpdateBanner, pData->updateBannerRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringImportSettings, pData->importSettingsRect));
        anyHover |= (pData->currentPage == 0 && checkHover(pData->isHoveringExportSettings, pData->exportSettingsRect));
        anyHover |= (pData->currentPage == 3 && checkHover(pData->isHoveringFpsCapper, pData->fpsCapperDropdownRect));
        anyHover |= PtInRect(&pData->multiInstanceToggleRect, pt) && pData->currentPage == 0;
        anyHover |= PtInRect(&pData->fishstrapToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->autoUpdateToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->autoStartToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoReconnectToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoResetToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoHideToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoOpacityToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->autoGridToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->bloxstrapIntegrationToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->iCanForgetToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->doNotSleepToggleRect, pt) && pData->currentPage == 1;
        anyHover |= PtInRect(&pData->unlockFpsOnFocusToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->autoMuteToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->unmuteOnFocusToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->legacyUiToggleRect, pt) && pData->currentPage == 3;
        anyHover |= PtInRect(&pData->statusBarToggleRect, pt) && pData->currentPage == 3;
        if (pData->currentPage == 5) {
            anyTextHover |= checkHover(pData->isHoveringDiscordWebhookInput, pData->discordWebhookInputRect);
            anyHover |= checkHover(pData->isHoveringDiscordWebhookPaste, pData->discordWebhookPasteRect);
            anyHover |= checkHover(pData->isHoveringDiscordWebhookClear, pData->discordWebhookClearRect);
            anyHover |= checkHover(pData->isHoveringDiscordWebhookTest, pData->discordWebhookTestRect);
            anyHover |= PtInRect(&pData->discordWebhookEnableToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordNotifyStartToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordNotifyActionToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordNotifyReconnectToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordNotifyErrorsToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordDisableEmbedToggleRect, pt) != FALSE;
            anyHover |= PtInRect(&pData->discordMentionOnErrorsToggleRect, pt) != FALSE;
        }
        if (pData->currentPage == 4) {
            anyHover |= checkHover(pData->isHoveringUtilsShowRoblox, pData->utilsShowRobloxRect);
            anyHover |= checkHover(pData->isHoveringUtilsHideRoblox, pData->utilsHideRobloxRect);
            anyHover |= checkHover(pData->isHoveringUtilsGridSnap, pData->utilsGridSnapRect);
            anyHover |= checkHover(pData->isHoveringUtilsWindowOpacity, pData->utilsWindowOpacityRect);
            anyHover |= checkHover(pData->isHoveringUtilsCloseAll, pData->utilsCloseAllRect);
            anyHover |= checkHover(pData->isHoveringUtilsResetAll, pData->utilsResetAllRect);
            anyHover |= checkHover(pData->isHoveringUtilsTestAction, pData->utilsTestActionRect);
            anyHover |= checkHover(pData->isHoveringUtilsMuteToggle, pData->utilsMuteToggleRect);
            anyHover |= checkHover(pData->isHoveringUtilsFpsToggle, pData->utilsFpsToggleRect);
        }

        int newHoveringNavItem = -1;
        for (size_t i = 0; i < pData->navItems.size(); ++i) {
            if (PtInRect(&pData->navItems[i].rect, pt)) {
                newHoveringNavItem = (int)i;
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

        if (anyTextHover) SetCursor(pData->hCursorText);
        else SetCursor(anyHover ? pData->hCursorHand : pData->hCursorArrow);

        if ((anyHover || anyTextHover) && !pData->isTrackingMouse) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            pData->isTrackingMouse = true;
        }
        if (anyHover || anyTextHover) {
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE:
        pData->isHoveringIcon = pData->isHoveringClose = pData->isHoveringStart = pData->isHoveringReset = pData->isHoveringInterval = pData->isHoveringAction = pData->isHoveringUserSafe = pData->isHoveringRestore = false;
        pData->isHoveringMultiInstanceInterval = false;
        pData->isHoveringDiscord = false;
        pData->isHoveringUpdateBanner = false;
        pData->hoveringNavItem = -1; pData->isHoveringResetStats = false;
        pData->isHoveringFpsCapper = false; 
        pData->isHoveringDiscordWebhookInput = false;
        pData->isHoveringDiscordWebhookPaste = false;
        pData->isHoveringDiscordWebhookClear = false;
        pData->isHoveringDiscordWebhookTest = false;
        pData->isHoveringUtilsShowRoblox = false;
        pData->isHoveringUtilsHideRoblox = false;
        pData->isHoveringUtilsGridSnap = false;
        pData->isHoveringUtilsWindowOpacity = false;
        pData->isHoveringUtilsCloseAll = false;
        pData->isHoveringUtilsResetAll = false;
        pData->isHoveringUtilsTestAction = false;
        pData->isHoveringUtilsMuteToggle = false;
        pData->isHoveringUtilsFpsToggle = false;
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
    case WM_ERASEBKGND:
        return 1;
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

        if (pData->currentPage >= 0 && pData->currentPage < (int)pData->navItems.size()) {
            const RECT& targetRect = pData->navItems[pData->currentPage].rect;
            float targetX = (float)targetRect.left;
            float targetW = (float)(targetRect.right - targetRect.left);
            animateFloat(pData->navIndicatorX, targetX);
            animateFloat(pData->navIndicatorWidth, targetW);
            if (abs(pData->navIndicatorX - targetX) > 0.5f || abs(pData->navIndicatorWidth - targetW) > 0.5f) needsRedraw = true;
        }


        animateToggle(pData->multiInstanceAnim, g_multiSupport.load());
        animateToggle(pData->fishstrapAnim, g_fishstrapSupport.load());
        animateToggle(pData->autoUpdateAnim, g_autoUpdate.load());
        animateToggle(pData->autoStartAnim, g_autoStartAfk.load());
        animateToggle(pData->autoReconnectAnim, g_autoReconnect.load());
        animateToggle(pData->autoResetAnim, g_autoReset.load());
        animateToggle(pData->autoHideAnim, g_autoHideRoblox.load());
        animateToggle(pData->autoOpacityAnim, g_autoOpacity.load());
        animateToggle(pData->autoGridAnim, g_autoGrid.load());
        animateToggle(pData->unlockFpsOnFocusAnim, g_unlockFpsOnFocus.load());
        animateToggle(pData->bloxstrapIntegrationAnim, g_bloxstrapIntegration.load());
        animateToggle(pData->legacyUiAnim, g_useLegacyUi.load());
        animateToggle(pData->statusBarAnim, g_statusBarEnabled.load());
        animateToggle(pData->iCanForgetAnim, g_afkReminderEnabled.load());
        animateToggle(pData->doNotSleepAnim, g_doNotSleep.load());
        animateToggle(pData->autoMuteAnim, g_autoMute.load());
        animateToggle(pData->unmuteOnFocusAnim, g_unmuteOnFocus.load());
        animateToggle(pData->utilsWindowOpacityAnim, IsUtilsSessionWindowOpacityEnabled());
        animateToggle(pData->utilsMuteAnim, IsUtilsSessionMuteEnabled());
        animateToggle(pData->utilsFpsAnim, IsUtilsSessionFpsLimitEnabled());
        animateToggle(pData->discordWebhookEnableAnim, g_discordWebhookEnabled.load());
        animateToggle(pData->discordNotifyStartAnim, g_discordNotifyStart.load());
        animateToggle(pData->discordNotifyStopAnim, g_discordNotifyStop.load());
        animateToggle(pData->discordNotifyActionAnim, g_discordNotifyAction.load());
        animateToggle(pData->discordNotifyReconnectAnim, g_discordNotifyReconnect.load());
        animateToggle(pData->discordNotifyResetAnim, g_discordNotifyReset.load());
        animateToggle(pData->discordNotifyErrorsAnim, g_discordNotifyErrors.load());
        animateToggle(pData->discordDisableEmbedAnim, g_discordDisableEmbed.load());
        animateToggle(pData->discordMentionOnErrorsAnim, g_discordMentionOnErrors.load());
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
            DeleteObject(pData->hFontNav); DeleteObject(pData->hFontNavSelected); DeleteObject(pData->hFontNavIcon);
            DestroyCursor(pData->hCursorHand); DestroyCursor(pData->hCursorArrow); DestroyCursor(pData->hCursorText);
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
        int totalBtnW = buttons.size() == 1
            ? dlgW
            : dlgW;
        int btnY = dlgH - (button_pane_height - 10);
        int currentX = 0;

        for (size_t i = 0; i < buttons.size(); ++i)
        {
            const auto& btn = buttons[i];
            bool isDefault = false;
            UINT defBtn = (pData->params->type & MB_DEFMASK);
            if ((defBtn == MB_DEFBUTTON1 && i == 0) || (defBtn == MB_DEFBUTTON2 && i == 1) || (defBtn == MB_DEFBUTTON3 && i == 2))
            {
                isDefault = true;
            }
            int resolvedBtnW = buttons.size() == 1
                ? dlgW
                : (i == buttons.size() - 1 ? (dlgW - currentX) : (dlgW / (int)buttons.size()));
            int resolvedBtnH = button_pane_height - 10;
            int resolvedBtnY = btnY;
            RECT btnRect = { currentX, resolvedBtnY, currentX + resolvedBtnW, resolvedBtnY + resolvedBtnH };
            pData->buttonRects.push_back({ btn.first, btnRect });
            if (isDefault)
                SetFocus(hwnd); 
            currentX += buttons.size() == 1 ? (resolvedBtnW + btnGap) : resolvedBtnW;
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

        wchar_t caption[256];
        GetWindowTextW(hwnd, caption, 128);
        Popup_DrawChrome(&g, memDC, clientRect, pData->closeButtonRect, pData->hFontTitle, caption, pData->isHoveringClose);

        Font textFont(memDC, pData->hFontText);
        SolidBrush textBrush(Color(255, 220, 220, 220));
        RectF textRectF((REAL)20, (REAL)46, (REAL)(clientRect.right - 40), (REAL)(clientRect.bottom - 98));
        
        StringFormat sf;
        sf.SetAlignment(StringAlignmentNear);
        sf.SetLineAlignment(StringAlignmentNear);
        g.DrawString(pData->params->text, -1, &textFont, textRectF, &sf, &textBrush);

        for (const auto& btn : pData->buttonRects) {
            const wchar_t* text = L"";
            if (btn.first == IDOK) text = L"OK";
            else if (btn.first == IDCANCEL) text = L"Cancel";
            else if (btn.first == IDYES) text = L"Yes";
            else if (btn.first == IDNO) text = L"No";

            bool isPrimary = (btn.first == IDOK || btn.first == IDYES);
            Popup_DrawActionButton(&g, memDC, btn.second, pData->hFontText, text, pData->hoveringButton == btn.first, isPrimary);
        }

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
    if (g_userSafeMode > 0 || g_afkReminderEnabled.load())
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
            g_afkStartTime = GetTickCount64();
            g_isAfkStarted = true;
            CreateTrayMenu(true);
            UpdateTrayIcon();
            QueueDiscordWebhookEvent(DiscordWebhookEvent::Started, L"Started on launch.", false);
            if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
                ApplyAutoRobloxWindowLayout();
            }
        }
        else
        {
            UpdateSplashStatus(L"Roblox not found for auto-start.");
        }
    }

    if (GetEffectiveFpsLimit() > 0) {
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
                    FinalizeAfkSession();
                    g_isAfkStarted = false;
                    if (ShouldMuteRobloxNow()) { MuteAllRoblox(true); } else { MuteAllRoblox(false); }
                    g_unmutedPid = 0;
                    QueueDiscordWebhookEvent(DiscordWebhookEvent::Stopped, L"Stopped: Roblox not found.", false);
                    QueueStatusBarOverlay(L"Auto-stopped: Roblox not found", 2200, user);
                    CreateTrayMenu(false); UpdateTrayIcon(); ShowTrayNotification(L"AntiAFK-RBX | Auto-Stopped", L"Roblox window not found. AntiAFK is now off."); continue; 
                }
                else
                {
                    ShowTrayNotification(L"AntiAFK Error", L"Roblox window not found!");
                    QueueDiscordWebhookEvent(DiscordWebhookEvent::Error, L"Roblox not found.", false);
                    QueueStatusBarOverlay(L"Roblox window not found", 2200, user);
                }
            }
            else
            {
                bool autoReconnectTriggered = false;
                if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load()) RefreshRobloxWindowOpacity(false);
                if (g_autoGrid.load()) ApplyAutoRobloxWindowLayout();
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
                            break;
                        }

                        if (elapsedSeconds >= MAX_WAIT_TIME)
                        {
                            break;
                        }

                        if (!notifiedUser && elapsedSeconds > 5)
                        {
                            QueueStatusBarOverlay(L"Waiting for inactivity", 2000, wins.front());
                            notifiedUser = true;
                        }

                        Sleep(500);

                        if (g_stopThread.load())
                            return;
                    }
                }

                bool wasFpsCapperPaused = PauseFpsCapperBeforeAction(FPS_CAPPER_PRE_ACTION_PAUSE_MS, true);

                QueueStatusBarOverlay(L"Performing anti-AFK action", STATUS_BAR_ACTION_PENDING_DURATION, wins.front());

                bool cancelPendingAction = false;
                DWORD statusBarLeadWaited = 0;
                while (statusBarLeadWaited < STATUS_BAR_PRE_ACTION_DELAY)
                {
                    if (g_stopThread.load()) {
                        ResumeFpsCapperAfterAction(wasFpsCapperPaused);
                        return;
                    }
                    if (!g_isAfkStarted.load()) {
                        cancelPendingAction = true;
                        break;
                    }

                    DWORD waitChunk = (std::min)((DWORD)50, STATUS_BAR_PRE_ACTION_DELAY - statusBarLeadWaited);
                    Sleep(waitChunk);
                    statusBarLeadWaited += waitChunk;
                }

                if (cancelPendingAction) {
                    ResumeFpsCapperAfterAction(wasFpsCapperPaused);
                    continue;
                }

                if (g_multiSupport.load())
                {
                    for (size_t i = 0; i < wins.size(); i++)
                    {
                        HWND w = wins[i];
                        bool wasMinimized = IsIconic(w);
                        if (wasMinimized)
                            ShowWindow(w, SW_RESTORE);

                        SetForegroundWindow(w);
                        if (g_autoReconnect.load()) {
                            if (CheckForAutoReconnect(w)) {
                                g_autoReconnectsPerformed++;
                                autoReconnectTriggered = true;
                            }
                        }
                        for (int j = 0; j < 3; j++)
                        {
                            AntiAFK_Action(w);
                        }
                        if (g_autoReset.load()) {
                            AutoReset_Action(w);
                        }

                        if (wasMinimized)
                            ShowWindow(w, SW_MINIMIZE);
                        
                        if (g_multiInstanceInterval > 0 && i < wins.size() - 1) {
                            Sleep(g_multiInstanceInterval);
                        }
                    }
                }
                else
                {
                    HWND w = wins.front();
                    bool wasMinimized = IsIconic(w);
                    if (wasMinimized)
                        ShowWindow(w, SW_RESTORE);

                    SetForegroundWindow(w);
                    if (g_autoReconnect.load()) {
                        if (CheckForAutoReconnect(w)) {
                            g_autoReconnectsPerformed++;
                            autoReconnectTriggered = true;
                        }
                    }
                    for (int i = 0; i < 3; i++)
                    {
                        AntiAFK_Action(w);
                    }
                    if (g_autoReset.load()) {
                        AutoReset_Action(w);
                    }

                    if (wasMinimized)
                    {
                        ShowWindow(w, SW_MINIMIZE);
                    }
                }

                ResumeFpsCapperAfterAction(wasFpsCapperPaused);

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

                QueueStatusBarOverlay(L"Performing anti-AFK action", STATUS_BAR_POST_ACTION_DURATION, wins.front());
                g_afkActionsPerformed++;
                g_lastAfkActionTimestamp = GetTickCount64();
                QueueDiscordWebhookEvent(DiscordWebhookEvent::Action, L"Cycle completed.", false);
                if (autoReconnectTriggered) {
                    QueueDiscordWebhookEvent(DiscordWebhookEvent::AutoReconnect, L"Reconnect triggered.", false);
                }
            }
            g_updateInterval = false;

            int waitMs = 0;
            const int focusCheckInterval = 500;
            const int totalWaitMs = g_selectedTime * 1000;
            DWORD previousActivePid = 0;
            while (waitMs < totalWaitMs) {
                if (g_stopThread.load() || !g_isAfkStarted.load()) break;

                if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
                    ApplyAutoRobloxWindowLayout();
                }

                if (g_unmuteOnFocus.load() && g_autoMute.load() && g_utilsMuteOverride.load() == -1) {
                    HWND fg = GetForegroundWindow();
                    DWORD currentFgPid = 0;
                    GetWindowThreadProcessId(fg, &currentFgPid);

                    bool fgIsRoblox = false;
                    for (HWND w : wins) {
                        DWORD wPid = 0;
                        GetWindowThreadProcessId(w, &wPid);
                        if (wPid == currentFgPid) { fgIsRoblox = true; break; }
                    }

                    if (fgIsRoblox && currentFgPid > 0 && currentFgPid != g_unmutedPid.load()) {
                        if (previousActivePid > 0 && previousActivePid != currentFgPid) {
                            MuteProcessByPid(previousActivePid, true);
                        }
                        MuteProcessByPid(currentFgPid, false);
                        previousActivePid = currentFgPid;
                        g_unmutedPid = currentFgPid;
                    } else if (!fgIsRoblox && previousActivePid > 0) {
                        MuteProcessByPid(previousActivePid, true);
                        previousActivePid = 0;
                    }
                }

                bool notified = g_cv.wait_for(lock, std::chrono::milliseconds(focusCheckInterval), [] { return g_stopThread.load() || g_updateInterval.load() || !g_isAfkStarted.load(); });
                if (g_updateInterval.load()) break;
                waitMs += focusCheckInterval;
            }
            MuteUnmutedPid();
            g_unmutedPid = 0;
        }
        else
        {
            if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
                ApplyAutoRobloxWindowLayout();
            }

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
                            QueueStatusBarOverlay(L"AntiAFK auto-started", 2000, wins.front());
                            QueueDiscordWebhookEvent(DiscordWebhookEvent::Started, L"Started automatically.", false);
                            if (ShouldMuteRobloxNow()) {
                                MuteAllRoblox(true);
                            }
                            if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
                                ApplyAutoRobloxWindowLayout();
                            }
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
            auto idleWait = (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load() || g_autoStartAfk.load()) ? 1000ms : 5000ms;
            g_cv.wait_for(lock, idleWait, [] { return g_stopThread.load() || g_isAfkStarted.load(); });
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
                ResetStatisticsCounters();
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
                CreateTrayMenu(g_isAfkStarted.load());
                ShowStatusBarOverlay(L"Statistics reset", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
        }
        break;
    case WM_APP + 2:
        ShowTutorialDialog(hwnd);
        return 0;
    case WM_APP_SHOW_STATUS_BAR:
    {
        StatusBarPayload* payload = reinterpret_cast<StatusBarPayload*>(wParam);
        if (payload) {
            ShowStatusBarOverlay(payload->text, payload->durationMs, payload->anchorWindow);
            delete payload;
        }
        return 0;
    }
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
            break;
        }
        case ID_LINKWIKI:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX/wiki", NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        case ID_LINKGITHUB:
        case ID_UPDATE_AVAILABLE:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        case ID_LINKSF:
        {
            ShellExecute(NULL, L"open", L"https://antiafk-rbx.sourceforge.io", NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        case ID_INFORMATION:
        {
            ShellExecute(NULL, L"open", L"https://github.com/Agzes/AntiAFK-RBX", NULL, NULL, SW_SHOWNORMAL);
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

                 if (ShouldMuteRobloxNow()) {
                     MuteAllRoblox(true);
                 }

                 if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
                     ApplyAutoRobloxWindowLayout();
                 }

                 if (g_autoHideRoblox.load()) {
                      for (HWND w : wins) {
                          ShowWindow(w, SW_HIDE);
                      }
                  }

                 ShowStatusBarOverlay(L"AntiAFK started", 1800, wins.front());
                  QueueDiscordWebhookEvent(DiscordWebhookEvent::Started, L"Started manually.", false);
             }
             else
             {
                 ShowTrayNotification(L"Error", L"Roblox window not found!");
                 ShowStatusBarOverlay(L"Roblox window not found", 2200, GetForegroundWindow());
                 QueueDiscordWebhookEvent(DiscordWebhookEvent::Error, L"Start failed: Roblox not found.", false);
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
            if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
                FinalizeAfkSession();
                SaveSettings();
            }
            g_isAfkStarted = false;
            g_afkReminderState = 0;
            auto wins_stop = FindAllRobloxWindows(true);
           g_manuallyStoppedPids.clear();
            for (HWND w : wins_stop) {
                DWORD pid; GetWindowThreadProcessId(w, &pid);
                if (std::find(g_manuallyStoppedPids.begin(), g_manuallyStoppedPids.end(), pid) == g_manuallyStoppedPids.end()) {
                    g_manuallyStoppedPids.push_back(pid);
                }
                
                if (g_autoHideRoblox.load()) {
                    ShowWindow(w, SW_SHOW);
                }
                if (ShouldMuteRobloxNow()) {
                    MuteAllRoblox(true);
                } else {
                    MuteAllRoblox(false);
                }
                g_unmutedPid = 0;
            }
            RefreshRobloxWindowOpacity(false);
            CreateTrayMenu(false);
            UpdateTrayIcon();
            ShowStatusBarOverlay(L"AntiAFK stopped", 1800, wins_stop.empty() ? GetForegroundWindow() : wins_stop.front());
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
            QueueDiscordWebhookEvent(DiscordWebhookEvent::Stopped, L"Stopped manually.", false);
            break; 
        }

        case ID_UTILS_SHOW_ALL:
        {
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            else
            {
                ShowAllRobloxWindows_Multi();
                ShowStatusBarOverlay(L"All Roblox windows shown", 1600, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : wins.front());
            }
            break;
        }
        case ID_UTILS_HIDE_ALL:
        {
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            else
            {
                HideAllRobloxWindows();
                ShowStatusBarOverlay(L"All Roblox windows hidden", 1600, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            break;
        }
        case ID_SHOW_WINDOW:
        {
            bool shown = false;
            HWND anchor = g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow();
            if (!g_multiSupport.load())
            {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx)
                {
                    ShowWindow(rbx, SW_SHOW);
                    shown = true;
                    anchor = rbx;
                }
                else if (g_fishstrapSupport.load())
                {
                    rbx = FindWindowByProcessName(L"eurotrucks2.exe");
                    if (rbx)
                    {
                        ShowWindow(rbx, SW_SHOW);
                        shown = true;
                        anchor = rbx;
                    }
                    else
                    {
                        ShowTrayNotification(L"Error", L"Roblox window not found!");
                        ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                    }
                }
                else
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                    ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                }
            }
            else
            {
                auto wins = FindAllRobloxWindows(true);
                if (wins.empty())
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                    ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                }
                else
                {
                    ShowAllRobloxWindows_Multi();
                    shown = true;
                    anchor = wins.front();
                }
            }

            if (shown)
            {
                ShowStatusBarOverlay(L"Roblox window shown", 1600, anchor);
            }
            break;
        }
        case ID_HIDE_WINDOW:
        {
            bool hidden = false;
            HWND anchor = g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow();
            if (!g_multiSupport.load())
            {
                HWND rbx = FindWindowByProcessName(L"RobloxPlayerBeta.exe");
                if (rbx)
                {
                    ShowWindow(rbx, SW_HIDE);
                    hidden = true;
                    anchor = rbx;
                }
                else if (g_fishstrapSupport.load())
                {
                    rbx = FindWindowByProcessName(L"eurotrucks2.exe");
                    if (rbx)
                    {
                        ShowWindow(rbx, SW_HIDE);
                        hidden = true;
                        anchor = rbx;
                    }
                    else
                    {
                        ShowTrayNotification(L"Error", L"Roblox window not found!");
                        ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                    }
                }
                else
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                    ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                }
            }
            else
            {
                auto wins = FindAllRobloxWindows(true);
                if (wins.empty())
                {
                    ShowTrayNotification(L"Error", L"Roblox window not found!");
                    ShowStatusBarOverlay(L"Roblox window not found", 1800, anchor);
                }
                else
                {
                    for (HWND w : wins)
                        ShowWindow(w, SW_HIDE);
                    hidden = true;
                    anchor = wins.front();
                }
            }

            if (hidden)
            {
                ShowStatusBarOverlay(L"Roblox window hidden", 1600, anchor);
            }
            break;
        }
        case ID_GRID_SNAP:
        {
            auto wins = FindAllRobloxWindows(true);
            if (wins.empty())
            {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            else
            {
                GridSnapRobloxWindows();
                ShowStatusBarOverlay(L"Roblox windows arranged", 1800, wins.front());
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

                g_lastAfkActionTimestamp = GetTickCount64();
                ShowStatusBarOverlay(L"Test AntiAFK action sent", 1800, wins.front());
            }
            else
            {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
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
                            if (g_useLegacyUi.load()) {
                                ShowTrayNotification(L"UI Mode Changed", L"Switched to Legacy (Tray) mode.");
                            }
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
        case ID_STATUS_BAR:
            g_statusBarEnabled = !g_statusBarEnabled.load();
            if (!g_statusBarEnabled.load()) {
                HideStatusBarOverlay(false);
            }
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            if (g_statusBarEnabled.load()) {
                ShowStatusBarOverlay(L"Status Bar enabled", 1400, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : NULL);
            }
            break;
        case ID_USER_SAFE_OFF:
        case ID_USER_SAFE_LEGACY:
        case ID_USER_SAFE_BETA:
            g_userSafeMode = LOWORD(wParam) - ID_USER_SAFE_OFF;
            if (g_userSafeMode > 0)
            {
                if (!g_monitorThreadRunning.load()) StartActivityMonitor();
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
        case ID_AUTO_RESET:
            g_autoReset = !g_autoReset.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_HIDE:
            g_autoHideRoblox = !g_autoHideRoblox.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_OPACITY:
            g_autoOpacity = !g_autoOpacity.load();
            RefreshRobloxWindowOpacity(false);
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_GRID:
            g_autoGrid = !g_autoGrid.load();
            if (g_autoGrid.load()) {
                ApplyAutoRobloxWindowLayout();
            } else {
                std::lock_guard<std::mutex> guard(g_autoWindowLayoutMutex);
                g_lastAutoGridWindowSignature.clear();
            }
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_I_CAN_FORGET:
            g_afkReminderEnabled = !g_afkReminderEnabled.load();
            g_afkReminderState = 0;
            if (g_afkReminderEnabled.load() && !g_monitorThreadRunning.load()) {
                StartActivityMonitor();
            }
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_DO_NOT_SLEEP:
            g_doNotSleep = !g_doNotSleep.load();
            if (g_doNotSleep.load()) {
                SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
            } else {
                SetThreadExecutionState(ES_CONTINUOUS);
            }
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_AUTO_MUTE:
            g_autoMute = !g_autoMute.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_UNMUTE_ON_FOCUS:
            g_unmuteOnFocus = !g_unmuteOnFocus.load();
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
        case ID_WINDOW_OPACITY:
            g_windowOpacity = !g_windowOpacity.load();
            RefreshRobloxWindowOpacity(false);
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        case ID_UTILS_TOGGLE_WINDOW_OPACITY:
        {
            bool enableOpacity = !IsUtilsSessionWindowOpacityEnabled();
            g_utilsWindowOpacityOverride = enableOpacity ? 1 : 0;
            RefreshRobloxWindowOpacity(false);
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            ShowStatusBarOverlay(enableOpacity ? L"Session opacity enabled" : L"Session opacity disabled", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            break;
        }
        case ID_UTILS_CLOSE_ALL:
        {
            int result = ShowDarkMessageBox(
                g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd,
                L"Close all detected Roblox clients?\n\nAntiAFK-RBX will first ask windows to close, then force-close any remaining Roblox processes.",
                L"Close All Roblox",
                MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

            if (result == IDYES)
            {
                bool hadRoblox = !FindAllRobloxProcessIds().empty();
                int terminatedCount = CloseAllRobloxInstances();
                if (hadRoblox || terminatedCount > 0) {
                    ShowStatusBarOverlay(L"Closed all Roblox clients", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
                } else {
                    ShowStatusBarOverlay(L"No Roblox processes found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
                }
            }
            break;
        }
        case ID_UTILS_RESET_ALL:
        {
            if (ExecuteRobloxWindowActionForAll(AutoReset_Action, 1)) {
                ShowStatusBarOverlay(L"Reset sent to all Roblox windows", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
                QueueDiscordWebhookEvent(DiscordWebhookEvent::AutoReset, L"Manual reset sent to all Roblox windows.", false);
            } else {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            break;
        }
        case ID_UTILS_TEST_ACTION:
        {
            if (ExecuteRobloxWindowActionForAll(AntiAFK_Action, 3)) {
                g_lastAfkActionTimestamp = GetTickCount64();
                ShowStatusBarOverlay(L"Test AntiAFK action sent", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            } else {
                ShowTrayNotification(L"Error", L"Roblox window not found!");
                ShowStatusBarOverlay(L"Roblox window not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            break;
        }
        case ID_UTILS_TOGGLE_MUTE:
        {
            auto pids = FindAllRobloxProcessIds();
            if (pids.empty()) {
                ShowTrayNotification(L"Error", L"Roblox process not found!");
                ShowStatusBarOverlay(L"Roblox process not found", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
                break;
            }

            bool shouldMute = !IsUtilsSessionMuteEnabled();
            g_utilsMuteOverride = shouldMute ? 1 : 0;
            MuteAllRoblox(shouldMute);
            if (!shouldMute) {
                g_unmutedPid = 0;
            }
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            ShowStatusBarOverlay(shouldMute ? L"Roblox muted" : L"Roblox unmuted", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            break;
        }
        case ID_UTILS_TOGGLE_FPS:
        {
            bool shouldLimit = !IsUtilsSessionFpsLimitEnabled();
            int sessionFpsLimit = g_fpsLimit > 0 ? g_fpsLimit : 5;
            g_utilsFpsLimitOverride = shouldLimit ? sessionFpsLimit : -1;
            RestartFpsCapperForEffectiveLimit();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            if (shouldLimit) {
                wchar_t fpsMessage[80];
                swprintf_s(fpsMessage, L"Session FPS limit set to %d", sessionFpsLimit);
                ShowStatusBarOverlay(fpsMessage, 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            } else {
                ShowStatusBarOverlay(L"Session FPS override disabled", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : GetForegroundWindow());
            }
            break;
        }
        case ID_RESET_STATS:
        {
            int result = ShowDarkMessageBox(
                g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd,
                L"Are you sure you want to reset all statistics?\nThis action cannot be undone.",
                L"Reset Statistics",
                MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

            if (result == IDYES)
            {
                ResetStatisticsCounters();
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
                CreateTrayMenu(g_isAfkStarted.load());
                ShowStatusBarOverlay(L"Statistics reset", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            break;
        }
        case ID_DISCORD_WEBHOOK_ENABLE:
            g_discordWebhookEnabled = !g_discordWebhookEnabled.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordWebhookEnabled.load() ? L"Discord webhook enabled" : L"Discord webhook disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_NOTIFY_START:
            g_discordNotifyStart = !g_discordNotifyStart.load();
            g_discordNotifyStop = g_discordNotifyStart.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordNotifyStart.load() ? L"Webhook Start / Stop enabled" : L"Webhook Start / Stop disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_NOTIFY_ACTION:
            g_discordNotifyAction = !g_discordNotifyAction.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordNotifyAction.load() ? L"Webhook Action enabled" : L"Webhook Action disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_NOTIFY_RECONNECT:
            g_discordNotifyReconnect = !g_discordNotifyReconnect.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordNotifyReconnect.load() ? L"Webhook Reconnect enabled" : L"Webhook Reconnect disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_NOTIFY_ERRORS:
            g_discordNotifyErrors = !g_discordNotifyErrors.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordNotifyErrors.load() ? L"Webhook Errors enabled" : L"Webhook Errors disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_DISABLE_EMBED:
            g_discordDisableEmbed = !g_discordDisableEmbed.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordDisableEmbed.load() ? L"Webhook embeds disabled" : L"Webhook embeds enabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_MENTION_ON_ERRORS:
            g_discordMentionOnErrors = !g_discordMentionOnErrors.load();
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(g_discordMentionOnErrors.load() ? L"Webhook mentions enabled" : L"Webhook mentions disabled", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_WEBHOOK_PASTE:
        {
            std::wstring clipboardText;
            if (!ReadUnicodeTextFromClipboard(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd, clipboardText))
            {
                ShowDarkMessageBox(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd, L"Clipboard does not contain Unicode text right now.", L"Discord Webhook", MB_OK | MB_ICONWARNING);
                ShowStatusBarOverlay(L"Clipboard text not available", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
                break;
            }

            SetDiscordWebhookUrl(clipboardText);
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(IsDiscordWebhookUrl(GetDiscordWebhookUrlCopy()) ? L"Webhook URL pasted" : L"Webhook URL pasted, but looks invalid", 1900, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        }
        case ID_DISCORD_WEBHOOK_CLEAR:
            SetDiscordWebhookUrl(L"");
            SaveSettings();
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            ShowStatusBarOverlay(L"Webhook URL cleared", 1700, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        case ID_DISCORD_WEBHOOK_TEST:
        {
            std::wstring webhookUrl = GetDiscordWebhookUrlCopy();
            if (!IsDiscordWebhookUrl(webhookUrl))
            {
                ShowDarkMessageBox(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd, L"Enter a valid Discord webhook URL first.", L"Discord Webhook", MB_OK | MB_ICONWARNING);
                ShowStatusBarOverlay(L"Webhook URL is invalid", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
                break;
            }

            std::wstring errorText;
            DWORD statusCode = 0;
            bool sent = SendDiscordWebhookRequest(
                webhookUrl,
                DiscordWebhookEvent::Test,
                L"Manual test message from the Legacy UI.",
                &errorText,
                &statusCode);

            if (sent)
            {
                wchar_t successMessage[96];
                swprintf_s(successMessage, L"Test message sent successfully (HTTP %lu).", statusCode);
                ShowDarkMessageBox(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd, successMessage, L"Discord Webhook", MB_OK | MB_ICONINFORMATION);
                ShowStatusBarOverlay(L"Test webhook sent", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            else
            {
                ShowDarkMessageBox(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd, errorText.empty() ? L"Failed to send the test webhook." : errorText.c_str(), L"Discord Webhook", MB_OK | MB_ICONERROR);
                ShowStatusBarOverlay(L"Test webhook failed", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            CreateTrayMenu(g_isAfkStarted.load());
            break;
        }
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
                RestartFpsCapperForEffectiveLimit();
                SaveSettings();
                CreateTrayMenu(g_isAfkStarted.load());
            }
            break;
        }
        case ID_FPS_CAP_CUSTOM:
        {
            ShowCustomFpsDialog(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            break;
        }
        case ID_MI_INTERVAL_0:
        case ID_MI_INTERVAL_1:
        case ID_MI_INTERVAL_3:
        case ID_MI_INTERVAL_5:
        case ID_MI_INTERVAL_10:
        {
            int command = LOWORD(wParam);
            switch (command) {
                case ID_MI_INTERVAL_0: g_multiInstanceInterval = 0; break;
                case ID_MI_INTERVAL_1: g_multiInstanceInterval = 1000; break;
                case ID_MI_INTERVAL_3: g_multiInstanceInterval = 3000; break;
                case ID_MI_INTERVAL_5: g_multiInstanceInterval = 5000; break;
                case ID_MI_INTERVAL_10: g_multiInstanceInterval = 10000; break;
            }
            SaveSettings();
            CreateTrayMenu(g_isAfkStarted.load());
            if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                InvalidateRect(g_hMainUiWnd, NULL, TRUE);
            }
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
                ShowStatusBarOverlay(L"Settings reset", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            break;
        }
        case ID_IMPORT_SETTINGS:
        {
            if (ImportSettingsFromFile(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd)) {
                CreateTrayMenu(g_isAfkStarted.load());
                UpdateTrayIcon();
                if (g_hMainUiWnd && IsWindow(g_hMainUiWnd)) {
                    InvalidateRect(g_hMainUiWnd, NULL, TRUE);
                }
                ShowStatusBarOverlay(L"Settings imported", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            else {
                ShowTrayNotification(L"Import Failed", L"Could not load settings from the selected file.");
                ShowStatusBarOverlay(L"Settings import failed", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            break;
        }
        case ID_EXPORT_SETTINGS:
        {
            if (ExportSettingsToFile(g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd)) {
                ShowStatusBarOverlay(L"Settings exported", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
            }
            else {
                ShowTrayNotification(L"Export Failed", L"Could not save settings to the selected file.");
                ShowStatusBarOverlay(L"Settings export failed", 1800, g_hMainUiWnd && IsWindow(g_hMainUiWnd) ? g_hMainUiWnd : hwnd);
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
            ResetRobloxSessionEffectsOnExit();
            if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
                FinalizeAfkSession();
            }
            if (g_hStatusBarWnd && IsWindow(g_hStatusBarWnd)) {
                DestroyWindow(g_hStatusBarWnd);
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
                    RestartFpsCapperForEffectiveLimit();
                    SaveSettings();
                    CreateTrayMenu(g_isAfkStarted.load());
                }
            }
            break;
        }
        break;
    case WM_DESTROY:
        ResetRobloxSessionEffectsOnExit();
        Shell_NotifyIcon(NIM_DELETE, &g_nid);
        if (g_isAfkStarted.load() && g_afkStartTime.load() > 0) {
            FinalizeAfkSession();
        }
        if (g_hStatusBarWnd && IsWindow(g_hStatusBarWnd)) {
            DestroyWindow(g_hStatusBarWnd);
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
    g_programLaunches++;
    SaveSettings();
    g_lastActivityTime = GetTickCount64();
    RefreshRobloxWindowOpacity(false);
    if (IsUtilsWindowOpacityEnabled() || g_autoOpacity.load() || g_autoGrid.load()) {
        ApplyAutoRobloxWindowLayout();
    }

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    bool arg_noSplash = false, arg_tray = false, arg_startAfk = false;
    bool arg_checkUpdatesAndExit = false, arg_resetSettingsAndExit = false;
    bool arg_force = false;
    bool arg_bloxstrapIntegration_cmd = false;
    bool arg_hasUtilityCommand = false;
    bool arg_utilsShowAll = false, arg_utilsHideAll = false, arg_utilsGridSnap = false;
    bool arg_utilsTestAction = false, arg_utilsResetAll = false, arg_utilsCloseAll = false;
    bool arg_utilsWindowOpacitySet = false, arg_utilsWindowOpacity = false;
    bool arg_utilsMuteSet = false, arg_utilsMute = false;
    bool arg_utilsFpsSet = false, arg_utilsFps = false;

    if (argv) {
        for (int i = 1; i < argc; ++i) {
            std::wstring arg = argv[i];

            if (arg == L"--force") arg_force = true;
            else if (arg == L"--no-splash") arg_noSplash = true;
            else if (arg == L"--tray" || arg == L"--no-ui") arg_tray = true;
            else if (arg == L"--start-afk") arg_startAfk = true;
            else if (arg == L"--legacy-ui") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_useLegacyUi = value;
            }
            else if (arg == L"--status-bar") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_statusBarEnabled = value;
            }
            else if (arg == L"--no-notifications") g_notificationsDisabled = true;
            else if (arg == L"--auto-update" || arg == L"--no-update-check") {
                bool value = (arg != L"--no-update-check");
                if (ReadOptionalOnOffArgument(argv, argc, i, value, value)) {
                    g_autoUpdate = value;
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
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_multiSupport = value;
            }
            else if (arg == L"--multi-instance-interval" && i + 1 < argc) {
                try {
                    int value = std::stoi(argv[++i]);
                    if (value == 0 || value == 1 || value == 3 || value == 5 || value == 10) {
                        g_multiInstanceInterval = value * 1000;
                    }
                } catch (...) {}
            }
            else if (arg == L"--auto-start") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoStartAfk = value;
            }
            else if (arg == L"--auto-reconnect") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoReconnect = value;
            }
            else if (arg == L"--auto-reset") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoReset = value;
            }
            else if (arg == L"--auto-hide") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoHideRoblox = value;
            }
            else if (arg == L"--auto-opacity") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoOpacity = value;
            }
            else if (arg == L"--auto-grid") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoGrid = value;
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
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_fishstrapSupport = value;
            }
            else if (arg == L"--bloxstrap-integration") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_bloxstrapIntegration = value;
                arg_bloxstrapIntegration_cmd = value;
            }
            else if (arg == L"--do-not-sleep") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_doNotSleep = value;
            }
            else if (arg == L"--unlock-fps-on-focus") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_unlockFpsOnFocus = value;
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
            else if (arg == L"--auto-mute") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_autoMute = value;
            }
            else if (arg == L"--unmute-on-focus") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_unmuteOnFocus = value;
            }
            else if (arg == L"--discord-webhook") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordWebhookEnabled = value;
            }
            else if (arg == L"--discord-webhook-url" && i + 1 < argc) {
                SetDiscordWebhookUrl(argv[++i]);
            }
            else if (arg == L"--discord-notify-start") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordNotifyStart = value;
                g_discordNotifyStop = value;
            }
            else if (arg == L"--discord-notify-action") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordNotifyAction = value;
            }
            else if (arg == L"--discord-notify-reconnect") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordNotifyReconnect = value;
            }
            else if (arg == L"--discord-notify-errors") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordNotifyErrors = value;
            }
            else if (arg == L"--discord-disable-embed") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordDisableEmbed = value;
            }
            else if (arg == L"--discord-mention-on-errors") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                g_discordMentionOnErrors = value;
            }
            else if (arg == L"--utils-show-all") {
                arg_hasUtilityCommand = true;
                arg_utilsShowAll = true;
            }
            else if (arg == L"--utils-hide-all") {
                arg_hasUtilityCommand = true;
                arg_utilsHideAll = true;
            }
            else if (arg == L"--utils-grid-snap") {
                arg_hasUtilityCommand = true;
                arg_utilsGridSnap = true;
            }
            else if (arg == L"--utils-window-opacity") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                arg_hasUtilityCommand = true;
                arg_utilsWindowOpacitySet = true;
                arg_utilsWindowOpacity = value;
            }
            else if (arg == L"--utils-mute") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                arg_hasUtilityCommand = true;
                arg_utilsMuteSet = true;
                arg_utilsMute = value;
            }
            else if (arg == L"--utils-fps") {
                bool value = true;
                ReadOptionalOnOffArgument(argv, argc, i, true, value);
                arg_hasUtilityCommand = true;
                arg_utilsFpsSet = true;
                arg_utilsFps = value;
            }
            else if (arg == L"--utils-test-action") {
                arg_hasUtilityCommand = true;
                arg_utilsTestAction = true;
            }
            else if (arg == L"--utils-reset-all") {
                arg_hasUtilityCommand = true;
                arg_utilsResetAll = true;
            }
            else if (arg == L"--utils-close-all") {
                arg_hasUtilityCommand = true;
                arg_utilsCloseAll = true;
            }
        }
        LocalFree(argv);
    }
    
    if (arg_hasUtilityCommand) {
        arg_noSplash = true;
        arg_tray = true;
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
        ShowDarkMessageBox(NULL, g_updateFound ? L"A new version is available." : (g_updateCheckFailed.load() ? L"Error to check updates." : L"You are using the latest version."), L"Update Check", MB_OK | MB_ICONINFORMATION);
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

    if (arg_hasUtilityCommand) {
        bool trayMenuNeedsRefresh = false;

        if (arg_utilsWindowOpacitySet) {
            g_utilsWindowOpacityOverride = arg_utilsWindowOpacity ? 1 : 0;
            RefreshRobloxWindowOpacity(false);
            trayMenuNeedsRefresh = true;
            ShowStatusBarOverlay(arg_utilsWindowOpacity ? L"Session opacity enabled" : L"Session opacity disabled", 1800, g_hwnd);
        }

        if (arg_utilsMuteSet) {
            g_utilsMuteOverride = arg_utilsMute ? 1 : 0;
            if (!arg_utilsMute) {
                g_unmutedPid = 0;
            }
            MuteAllRoblox(arg_utilsMute);
            trayMenuNeedsRefresh = true;
            ShowStatusBarOverlay(arg_utilsMute ? L"Roblox muted" : L"Roblox unmuted", 1800, g_hwnd);
        }

        if (arg_utilsFpsSet) {
            g_utilsFpsLimitOverride = arg_utilsFps ? (g_fpsLimit > 0 ? g_fpsLimit : 5) : -1;
            RestartFpsCapperForEffectiveLimit();
            trayMenuNeedsRefresh = true;
            if (arg_utilsFps) {
                wchar_t fpsMessage[80];
                swprintf_s(fpsMessage, L"Session FPS limit set to %d", g_utilsFpsLimitOverride.load());
                ShowStatusBarOverlay(fpsMessage, 1800, g_hwnd);
            } else {
                ShowStatusBarOverlay(L"Session FPS override disabled", 1800, g_hwnd);
            }
        }

        if (trayMenuNeedsRefresh) {
            CreateTrayMenu(g_isAfkStarted.load());
        }

        if (arg_utilsShowAll) {
            SendMessage(g_hwnd, WM_COMMAND, ID_UTILS_SHOW_ALL, 0);
        }
        if (arg_utilsHideAll) {
            SendMessage(g_hwnd, WM_COMMAND, ID_UTILS_HIDE_ALL, 0);
        }
        if (arg_utilsGridSnap) {
            SendMessage(g_hwnd, WM_COMMAND, ID_GRID_SNAP, 0);
        }
        if (arg_utilsTestAction) {
            SendMessage(g_hwnd, WM_COMMAND, ID_UTILS_TEST_ACTION, 0);
        }
        if (arg_utilsResetAll) {
            SendMessage(g_hwnd, WM_COMMAND, ID_UTILS_RESET_ALL, 0);
        }
        if (arg_utilsCloseAll) {
            SendMessage(g_hwnd, WM_COMMAND, ID_UTILS_CLOSE_ALL, 0);
        }
    }

    if (arg_tray) {
        g_tutorialShown = true; 
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

