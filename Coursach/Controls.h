#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>

#define BTNSTART 2
#define BTNSTOP 3
#define CHANNEL_RACK_START_ID 1000
#define CHANNEL_RACK_END_ID 1999
#define ID_BPM 101

using std::wstring, std::pair;

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
    BPMController(HWND parent, int x, int y);

    static BPMController* create(HWND parent, int x, int y);
    
    int getValue();
    
    void setValue(int value);

private:
    HWND hLabel, hEdit;
};

