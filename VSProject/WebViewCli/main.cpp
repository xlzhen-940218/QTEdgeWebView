// main.cpp
// 编译命令: cl.exe main.cpp WebView2Host.cpp user32.lib Ole32.lib OleAut32.lib
// 需要链接 WebView2Loader.dll.lib
#include <windows.h>
#include <tchar.h>
#include "EdgeWebView2.h"

// 全局变量
static TCHAR szWindowClass[] = _T("DesktopAppWithWebView2Class");
static TCHAR szTitle[] = _T("WebView2 Class Sample");
WebView2Host* g_webViewHost = nullptr; // 指向WebView2Host实例的指针

// 前向声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{    
    // >>>>> 关键修改：初始化COM库 <<<<<
    CoInitialize(NULL);
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("RegisterClassEx 失败!"), szTitle, MB_ICONERROR);
        return 1;
    }

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1200, 900,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd) {
        MessageBox(NULL, _T("CreateWindow 失败!"), szTitle, MB_ICONERROR);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // 在窗口创建时，实例化并创建WebView2
        g_webViewHost = new WebView2Host();
        g_webViewHost->Create(hWnd, [](bool success) {
            if (success && g_webViewHost) {
                // WebView2创建成功后，导航到bing
                g_webViewHost->Navigate(L"https://www.bing.com/");
            }
            
            });
        break;

    case WM_SIZE:
        // 当窗口大小改变时，调整WebView2的大小
        if (g_webViewHost) {
            g_webViewHost->Resize();
        }
        break;

    case WM_DESTROY:
        // 当窗口销毁时，清理WebView2资源并退出程序
        if (g_webViewHost) {
            g_webViewHost->Close();
            delete g_webViewHost;
            g_webViewHost = nullptr;
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}