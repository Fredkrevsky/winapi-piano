#include "Controls.h"

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