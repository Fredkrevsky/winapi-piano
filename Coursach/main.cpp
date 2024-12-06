#include "framework.h"
#include <windows.h>
#include "Resource.h"
#include "MainWindow.h"

#pragma comment(lib, "core\\lib\\x64\\fmod_vc.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MainWindow mainWindow(hInstance);
    if (!mainWindow.InitInstance(nCmdShow)) {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}