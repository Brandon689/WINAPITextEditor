#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")

#define IDC_MAIN_EDIT 101

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetEditFont(HWND hwndEdit);

HFONT g_hFont = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    static TCHAR szAppName[] = TEXT("SimpleEditor");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName, TEXT("Simple Text Editor"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hFont) DeleteObject(g_hFont);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;

    switch (message) {
    case WM_CREATE:
        hwndEdit = CreateWindow(TEXT("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, 0, 0, hwnd, (HMENU)IDC_MAIN_EDIT,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SetEditFont(hwndEdit);
        return 0;

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_DPICHANGED:
    {
        RECT* const prcNewWindow = (RECT*)lParam;
        SetWindowPos(hwnd,
            NULL,
            prcNewWindow->left,
            prcNewWindow->top,
            prcNewWindow->right - prcNewWindow->left,
            prcNewWindow->bottom - prcNewWindow->top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        // Update font for new DPI
        if (g_hFont) DeleteObject(g_hFont);
        SetEditFont(hwndEdit);
    }
    return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void SetEditFont(HWND hwndEdit) {
    HFONT hFont;
    HDC hdc = GetDC(hwndEdit);
    int dpi = GetDpiForWindow(hwndEdit);
    int fontSize = MulDiv(24, dpi, 96); // 11 point font size

    hFont = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        TEXT("Segoe UI"));

    if (hFont) {
        SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        if (g_hFont) DeleteObject(g_hFont);
        g_hFont = hFont;
    }

    ReleaseDC(hwndEdit, hdc);
}
