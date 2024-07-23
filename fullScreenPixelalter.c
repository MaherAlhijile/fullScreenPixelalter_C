#include <windows.h>

HWND window = NULL;
HDC screen, capture;
HBITMAP bits;
int pixelSize = 1;

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
        StretchBlt(capture, 0, 0, 1920 / pixelSize, 1080 / pixelSize, capture, 0, 0, 1920, 1080, SRCCOPY);
        StretchBlt(GetDC(window), 0, 0, 1920, 1080, capture, 0, 0, 1920 / pixelSize, 1080 / pixelSize, SRCCOPY);
        DeleteObject(bits);
        DeleteDC(capture);
        ReleaseDC(NULL, screen);

        return 0;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_UP)
        {

            pixelSize++;
            return 0;
        }

        if (wParam == VK_DOWN)
        {

            if (pixelSize > 1)
            {
                pixelSize--;
            }
            return 0;
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
