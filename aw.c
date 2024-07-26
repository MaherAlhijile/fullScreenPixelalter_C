#include <Windows.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


int frames;
clock_t start_time;
double elapsed_time, fps;
char str[255];


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HDC screen;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    frames = 0;
    start_time = clock();
    // Step 1: Register the window class
    const char *className = "SimpleWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    RECT rect = {0, 0, 1920, 1080};
    AdjustWindowRectEx(&rect, WS_POPUP, FALSE, WS_EX_TRANSPARENT);

    // Step 2: Create the window
    HWND hwnd = CreateWindowEx(

        WS_EX_TRANSPARENT,
        "SimpleWindowClass",
        "Simple Window",
        WS_POPUP | WS_VISIBLE,
        0, 0, rect.right - rect.left, rect.bottom - rect.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    long wl = GetWindowLong(hwnd, GWL_EXSTYLE);
    wl = wl | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    SetWindowLong(hwnd, GWL_EXSTYLE, wl);
    SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);

    // Step 4: Message loop
    SetTimer(hwnd, 0, 1, NULL);
    SetTimer(hwnd, 1, 1000, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER:
        switch (wParam)
        {
        case 0:

            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            break;
        case 1:
            fps = frames / elapsed_time;
            frames = 0;
            start_time = clock();

            break;
        default:

            break;
        }
        return 0;
    case WM_PAINT:
        frames++;
        elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;

        PAINTSTRUCT paint;
        HDC hdc = BeginPaint(hwnd, &paint);
        BitBlt(hdc, 0, 0, 1920, 1080, GetDC(NULL), 0, 0, SRCCOPY);
        RECT rect = {0, 0, 1920, 1080};
        sprintf(str, "%f", fps);
        DrawText(hdc, str, 3, &rect, DT_LEFT, DT_TOP);
        EndPaint(hwnd, &paint);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
