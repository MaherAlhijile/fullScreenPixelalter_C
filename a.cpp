#include <windows.h>
#include <iostream>

#pragma comment(lib, "gdi32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "User32")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HDC screen, capture, mainDC;
HBITMAP bits;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    screen = GetDC(NULL);
    capture = CreateCompatibleDC(screen);
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProc;

    if (!RegisterClassW(&wc))
    {
        return -1;
    }

    CreateWindowW(L"myWindowClass", L"Pixelater", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL); // Window style

    MSG msg = {0};

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
        bits = CreateCompatibleBitmap(screen, 1920, 1080);
        SelectObject(capture, bits);
        BitBlt(capture, 0, 0, 1920, 1080, capture, 0, 0, SRCCOPY);
        BitBlt(GetDC(hwnd), 0, 0, 400, 400, capture, 0, 0, SRCCOPY);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        break;
    }
}