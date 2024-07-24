#include <windows.h>

int pixelSize = 5;
int mode = 2;
int width = 1920;
int height = 1080;
int scaledWidth;
int scaledHeight;

HWND window = NULL;
HDC screen, capture;
HDC mainWin;
HBITMAP bits;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {

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

        bits = CreateCompatibleBitmap(screen, width, height);
        SelectObject(capture, bits);
        StretchBlt(capture, 0, 0, scaledWidth, scaledHeight, screen, 0, 0, width, height, SRCCOPY);
        StretchBlt(mainWin, 0, 0, width, height, capture, 0, 0, scaledWidth, scaledHeight, SRCCOPY);
        DeleteObject(bits);

        return 0;
    }

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
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

        default:
            break;
        }

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

        WS_EX_TRANSPARENT,

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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
