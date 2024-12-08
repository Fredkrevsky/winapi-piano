#ifndef PIANO_ROLL_H
#define PIANO_ROLL_H

#define NOMINMAX
#include <windows.h>
#include <vector>
#include <array>
#include <memory>
#include "Controls.h"

using std::vector, std::array, std::unique_ptr;

struct Note {
    int x, y;
    int length;
};

class PianoRoll final {
public:
    PianoRoll(HWND hwnd, int x, int y);

    vector<Note> getData();

    bool isClicked(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnRButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnPaint(HDC hdc);

private:

    void DrawGrid(HDC hdc);
    void DrawKeyboard(HDC hdc);
    void DrawNotes(HDC hdc);

    void AddNoteAt(int x, int y);
    void RemoveNoteAt(int x, int y);

    bool IsBlackKey(int midiNote);

    vector<Note> notes;
    array<unique_ptr<Button>, 5> durationButtons;
    int currentDuration = 4;

    const int CELL_WIDTH = 30;
    const int CELL_HEIGHT = 15;
    const int ROWS = 24;
    const int COLS = 32;
    const int MEASURE_DIVISION = 16;
    const int KEYBOARD_WIDTH = 100;
    const int GRID_LEFT = 50;

    const COLORREF WHITE_KEY_COLOR = RGB(255, 255, 255);
    const COLORREF BLACK_KEY_COLOR = RGB(50, 50, 50);
    const COLORREF NOTE_COLOR = RGB(144, 238, 144);
    const COLORREF NOTE_BORDER_COLOR = RGB(0, 128, 0);
    const COLORREF NOTE_LINE_COLOR = RGB(200, 200, 200);
    const COLORREF MEASURE_COLOR = RGB(0, 0, 0);
    const COLORREF BLACK_NOTE_STRIP_COLOR = RGB(230, 230, 230);

    const int BUTTON_HEIGHT = 40;
    const int BUTTON_WIDTH = 150;
    const int BUTTON_PADDING = 20;
    const int x;
    const int y;
};

#endif