#include <windows.h>
#include <stdio.h>
#include <time.h>

int pixelSize = 3;
int mode = 2;
int width;
int height;
int scaledWidth;
int scaledHeight;

BOOL fDraw = FALSE;
POINT ptPrevious;

HWND window = NULL;
HWND upper = NULL;
HDC upperDC;
HDC screen, capture;
HDC mainWin;
HBITMAP bits;

clock_t start, end;
double cpu_time_used;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    char pressedKey;
    // Declare a pointer to the KBDLLHOOKSTRUCTdsad
    KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
    switch (wParam)
    {
    case WM_KEYDOWN:

        // Check for specific keys
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            switch (pKeyBoard->vkCode)
            {
            case 0x31:
                mode = 1;
                break;
            case 0x32:
                mode = 2;
                break;
            case 0x33:
                mode = 3;
                break;
            case 0x34:
                mode = 4;
                break;
            case VK_ESCAPE:
                PostQuitMessage(0);
                break;
            case VK_UP:
                pixelSize++;
                scaledWidth = width / pixelSize;
                scaledHeight = height / pixelSize;

                break;
            case VK_DOWN:
                if (pixelSize > 1)
                {
                    pixelSize--;
                    scaledWidth = width / pixelSize;
                    scaledHeight = height / pixelSize;
                }
                break;
            }

        default:
            return CallNextHookEx(NULL, nCode, wParam, lParam);
            break;
        }
        return 0;
    }

    // according to winapi all functions which implement a hook must return by calling next hook
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CREATE:

        return 0;
    case WM_TIMER:
        switch (wParam)
        {
        case 0:

            width -= 10;

            break;
        case 1:
            height -= 10;
            break;
        default:

            break;
        }
        return 0;

    case WM_PAINT:
    {
        // PAINTSTRUCT ps;
        // mainWin = BeginPaint(window, &ps);
        bits = CreateCompatibleBitmap(screen, width, height);

        SelectObject(capture, bits);

        switch (mode)
        {
        case 1:
            SetStretchBltMode(capture, BLACKONWHITE);
            break;
        case 2:
            SetStretchBltMode(capture, COLORONCOLOR);
            break;
        case 3:
            SetStretchBltMode(capture, HALFTONE);
            break;
        case 4:
            SetStretchBltMode(capture, WHITEONBLACK);

            break;
        }

        StretchBlt(capture, 0, 0, scaledWidth, scaledHeight, screen, 0, 0, width, height, SRCCOPY);
        StretchBlt(mainWin, 0, 0, width, height, capture, 0, 0, scaledWidth, scaledHeight, SRCCOPY);
        DeleteObject(bits);

        return 0;
    }

    case WM_DESTROY:
        ReleaseDC(window, screen);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HHOOK test1 = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    scaledWidth = width / pixelSize;
    scaledHeight = height / pixelSize;
    screen = GetDC(NULL);
    capture = CreateCompatibleDC(screen);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SimpleWindowClass";

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    window = CreateWindowEx(

        WS_EX_TOOLWINDOW,

        "SimpleWindowClass",
        "Simple Window",
        WS_POPUP | WS_VISIBLE,
        0, 0, 1920, 1080,
        NULL,
        NULL,
        hInstance,
        NULL);

    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    long wl = GetWindowLong(window, GWL_EXSTYLE);
    wl = wl | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    SetWindowLong(window, GWL_EXSTYLE, wl);
    SetWindowDisplayAffinity(window, WDA_EXCLUDEFROMCAPTURE);
    mainWin = GetDC(window);

    if (window == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    SetTimer(window, 0, 1, NULL);
    SetTimer(window, 1, 1000, NULL);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
