#include <windows.h>
#include <stdlib.h>

int pixelSize = 5;
int mode = 1;


HWND window = NULL;
HDC screen, capture;
screen = NULL;
capture = NULL;
HBITMAP bits;

void setTransparent()
{
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    long wl = GetWindowLong(window, GWL_EXSTYLE);
    wl = wl | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    SetWindowLong(window, GWL_EXSTYLE, wl);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {

        screen = GetDC(NULL);
        capture = CreateCompatibleDC(screen);
        bits = CreateCompatibleBitmap(screen, 1920, 1080);
        SelectObject(capture, bits);
        SetWindowDisplayAffinity(window, WDA_EXCLUDEFROMCAPTURE);
        // Capture the screen into the bitmap
        BitBlt(capture, 0, 0, 1920, 1080, screen, 0, 0, SRCCOPY);
        // Apply pixelation effect by scaling down and then up
        // Adjust pixel size as needed
        switch (mode)
        {
        case (1):
        {
            SetStretchBltMode(capture, BLACKONWHITE);
            SetStretchBltMode(GetDC(window), BLACKONWHITE);
            break;
        }
        case (2):
        {
            SetStretchBltMode(capture, COLORONCOLOR);
            SetStretchBltMode(GetDC(window), COLORONCOLOR);
            break;
        }
        case (3):
        {
            SetStretchBltMode(capture, HALFTONE);
            SetStretchBltMode(GetDC(window), HALFTONE);
            break;
        }

        case (4):
        {
            SetStretchBltMode(capture, WHITEONBLACK);
            SetStretchBltMode(GetDC(window), WHITEONBLACK);
            break;
        }

        default:
            break;
        }
        StretchBlt(capture, 0, 0, 1920 / pixelSize, 1080 / pixelSize, capture, 0, 0, 1920, 1080, SRCCOPY);
        StretchBlt(GetDC(window), 0, 0, 1920, 1080, capture, 0, 0, 1920 / pixelSize, 1080 / pixelSize, SRCCOPY);
        DeleteObject(bits);
        DeleteDC(capture);
        ReleaseDC(NULL, screen);

        return 0;
    }

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_UP:
            pixelSize++;
            break;

        case VK_DOWN:
            if (pixelSize > 1)
            {
                pixelSize--;
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
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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
    setTransparent();

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
