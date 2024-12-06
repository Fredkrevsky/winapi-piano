#pragma once
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>

#define BTNSTART 2
#define BTNSTOP 3
#define CHANNEL_RACK_START_ID 1000
#define CHANNEL_RACK_END_ID 1999
#define ID_BPM 101

using namespace std;

class Button {
public:

    Button(LPCWSTR caption, int _x, int _y, int w, int h, int _id, HWND parent, DWORD flag);

    ~Button();

    static Button* createSimple(LPCWSTR caption, int x, int y, int id, HWND parent);

    static Button* createSmall(int x, int y, int id, HWND parent);

    static Button* createFreq(LPCWSTR caption, int x, int y, int id, HWND parent);

    int getId() const;

    void setText(wstring& text);

    pair<int, int> getPos() const;

    pair<int, int> getSize() const;

private:
    HWND btnwnd;
    int x, y, width, height, id;
};

class BPMController {
public:
    BPMController(HWND parent, int x, int y) {
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
    static BPMController* create(HWND parent, int x, int y) {
        return new BPMController(parent, x, y);
    }
    int getValue() {
        wchar_t buffer[16] = {};
        const size_t bufferSize = sizeof(buffer) / sizeof(buffer[0]);
        GetWindowText(hEdit, buffer, bufferSize - 1);

        if (buffer[0] == '\0') {
            throw std::invalid_argument("You must choose the tempo");
        }
        int number = stoi(buffer);
        if (number < 60 || number > 200) {
            throw std::invalid_argument("BPM must be from 60 to 200");
        }
        return number;
    }
    void setValue(int value) {
        wstring data = std::to_wstring(value);
        data.resize(16);
        SetWindowText(hEdit, data.c_str());
    }
private:
    HWND hLabel, hEdit;
};

