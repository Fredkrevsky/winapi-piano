#include "Controls.h"
#include <stdexcept>

Button::Button(LPCWSTR caption, int _x, int _y, int w, int h, int _id, HWND parent, DWORD flag)
    : x(_x), y(_y), width(w), height(h), id(_id) {
    btnwnd = CreateWindowEx(
        0, L"BUTTON", caption,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_FLAT | flag,
        x, y, width, height, parent, (HMENU)id,
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), NULL);
}

Button::~Button() { }

Button* Button::createSimple(LPCWSTR caption, int x, int y, int id, HWND parent) {
    return new Button(caption, x, y, 100, 30, id, parent, 0);
}

Button* Button::createFreq(LPCWSTR caption, int x, int y, int id, HWND parent) {
    return new Button(caption, x, y, 60, 30, id, parent, 0);
}

Button* Button::createSmall(int x, int y, int id, HWND parent) {
    return new Button(L"", x, y, 15, 20, id, parent, BS_OWNERDRAW);
}

int Button::getId() const {
    return id;
}

void Button::setText(wstring& text) {
    SetWindowText(btnwnd, text.c_str());
}

pair<int, int> Button::getPos() const {
    return { x, y };
}

pair<int, int> Button::getSize() const {
    return { width, height };
}


BPMController::BPMController(HWND parent, int x, int y) {
    hLabel = CreateWindow(
        L"STATIC", L"BPM:",
        WS_VISIBLE | WS_CHILD,
        x, y, 100, 20,
        parent, NULL, NULL, NULL
    );
    hEdit = CreateWindow(
        L"EDIT", L"100",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        x, y + 30, 100, 25,
        parent, (HMENU)ID_BPM, NULL, NULL
    );

    SendMessage(hEdit, EM_SETLIMITTEXT, 3, 0);
}

BPMController* BPMController::create(HWND parent, int x, int y) {
    return new BPMController(parent, x, y);
}

int BPMController::getValue() {
    wchar_t buffer[16] = {};
    const size_t bufferSize = sizeof(buffer) / sizeof(buffer[0]);
    GetWindowText(hEdit, buffer, bufferSize - 1);

    if (buffer[0] == '\0') {
        throw std::invalid_argument("You must choose the tempo");
    }
    int number = std::stoi(buffer);
    if (number < 60 || number > 200) {
        throw std::invalid_argument("BPM must be from 60 to 200");
    }
    return number;
}

void BPMController::setValue(int value) {
    wstring data = std::to_wstring(value);
    data.resize(16);
    SetWindowText(hEdit, data.c_str());
}