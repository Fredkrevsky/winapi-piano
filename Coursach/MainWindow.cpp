#include "MainWindow.h"
#include "Resource.h"
#include <windowsx.h>

#define BTN_START 201
#define BTN_STOP 202
#define BTN_RENDER 203

MainWindow::MainWindow(HINSTANCE hInstance) {
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWindow::WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"MainWindow";

    RegisterClassEx(&wcex);

    hwndMain = CreateWindow(L"MainWindow", L"Music Sequencer",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 1600, 800,
        nullptr, nullptr, hInstance, this);
}

MainWindow::~MainWindow() {
    UnregisterClass(L"MainWindow", GetModuleHandle(nullptr));
}

bool MainWindow::InitInstance(int nCmdShow) {

    if (!hwndMain) return false;

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    return true;
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        OnCreate(hwnd);
        break;
    case WM_PAINT: {
        OnPaint(hwnd);
        break;
    }
    case WM_KEYDOWN:
        OnKeyDown(hwnd, wParam, lParam);
        break;
    case WM_KEYUP:
        OnKeyUp(hwnd, wParam, lParam);
        break;
    case WM_DRAWITEM:
        channelRack->FillButtonColor(wParam, lParam);
        break;
    case WM_COMMAND:
        OnCommand(hwnd, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        OnLButtonDown(hwnd, wParam, lParam);
        break;
    case WM_RBUTTONDOWN:
        OnRButtonDown(hwnd, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void MainWindow::OnCreate(HWND hwnd) {
    manager.reset(new SoundManager(hwnd, 300, 50));
    bpmController.reset(new BPMController(hwnd, 50, 30));
    channelRack.reset(new ChannelRack(hwnd, 450, 50));
    pianoRoll.reset(new PianoRoll(hwnd, 300, 250));
    keyboardPiano.reset(new KeyboardPiano());
    keyboardPiano->start();

    buttonStart.reset(Button::createSimple(L"Start", 50, 150, BTN_START, hwnd));
    buttonStop.reset(Button::createSimple(L"Stop", 50, 200, BTN_STOP, hwnd));
    buttonSave.reset(Button::createSimple(L"Render", 50, 250, BTN_RENDER, hwnd));
}

void MainWindow::OnKeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    keyboardPiano->OnKeyDown(wParam);
}

void MainWindow::OnKeyUp(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    keyboardPiano->OnKeyUp(wParam);
}

void MainWindow::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(memDC, &clientRect, backgroundBrush);
    DeleteObject(backgroundBrush);

    pianoRoll->OnPaint(memDC);

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    EndPaint(hwnd, &ps);
}

void MainWindow::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int wmId{ static_cast<int>(wParam) };

    if (channelRack && channelRack->isClicked(wParam, lParam)) {
        channelRack->onClick(hwnd, wParam, lParam);
        SetFocus(hwnd);
    }
    else if (manager && manager->isClicked(wParam, lParam)) {
        try {
            manager->onClick(hwnd, wParam, lParam);
        }
        catch (std::exception e) {
            string text = e.what();
            wstring wtext(text.begin(), text.end());
            MessageBoxW(hwnd, wtext.c_str(), L"Îøèáêà", MB_OK | MB_ICONERROR);
        }
        SetFocus(hwnd);
    }
    else if (wmId == IDM_EXIT) {
        DestroyWindow(hwnd);
    }
    else if (wmId == BTN_START) {
        master();
        manager->play();
        SetFocus(hwnd);
    }
    else if (wmId == BTN_STOP) {
        manager->stop();
        SetFocus(hwnd);
    }
    else if (wmId == BTN_RENDER) {
        master();
        const wstring path = WavSound::saveFileDialog(hwnd);
        manager->saveToFile(path);
    }
    else if (pianoRoll && pianoRoll->isClicked(hwnd, wParam, lParam)){
        pianoRoll->OnCommand(hwnd, wParam, lParam);
    }
}

void MainWindow::master() {
    const auto drumData = channelRack->getData();
    const auto pianoData = pianoRoll->getData();
    int bpm{ 100 };
    try {
        bpm = bpmController->getValue();
    }
    catch (std::invalid_argument e) {
        bpmController->setValue(100);

        string text(e.what());
        wstring wtext(text.begin(), text.end());
        MessageBox(
            NULL,
            wtext.c_str(),
            (LPCWSTR)L"Error",
            MB_ICONERROR | MB_OK
        );
    }
    manager->setBpm(bpm);
    manager->master(drumData, pianoData);
}

void MainWindow::OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    if (pianoRoll->isClicked(hwnd, wParam, lParam)) {
        pianoRoll->OnLButtonDown(hwnd, wParam, lParam);
    }
    SetFocus(hwnd);
}

void MainWindow::OnRButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    if (pianoRoll->isClicked(hwnd, wParam, lParam)) {
        pianoRoll->OnRButtonDown(hwnd, wParam, lParam);
    }
    SetFocus(hwnd);
}

