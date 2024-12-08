#include "PianoRoll.h"
#include <algorithm>

#define BTN_DURATION 205

PianoRoll::PianoRoll(HWND hwnd, int _x, int _y) : x(_x), y(_y){
    const int yPos = 50 + ROWS * CELL_HEIGHT + 20;

    const array<LPCWSTR, 5> btnNames = { {
        L"Whole note",
        L"Half note",
        L"Quarter Note",
        L"Eighth Note",
        L"Sixteenth Note"
    } };

    std::ranges::for_each(durationButtons, [=, nameIndex=0, dx=50](auto& button) mutable {
        const LPCWSTR name = btnNames[nameIndex];
        const int btnId = BTN_DURATION + nameIndex;
        button.reset(Button::createSimple(name, x + 130 + dx, y + yPos, btnId, hwnd));
        nameIndex++;
        dx += 150;
    });
}

vector<Note> PianoRoll::getData() {
    return notes;
}

bool PianoRoll::isClicked(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int posx = LOWORD(lParam);
    const int posy = HIWORD(lParam);

    const int localX = posx - x - GRID_LEFT;
    const int localY = posy - y - 50;

    const int gridX = localX / CELL_WIDTH;
    const int gridY = localY / CELL_HEIGHT;

    if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
        return true;
    }

    const int btnId = static_cast<int>(wParam);

    return (btnId >= BTN_DURATION) && (btnId <= BTN_DURATION + 4);
}

void PianoRoll::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int btnId = static_cast<int>(wParam);
    const int index = btnId - BTN_DURATION;
    static const array<int, 5> durations = { {16, 8, 4, 2, 1} };
    currentDuration = durations[index];
}

void PianoRoll::OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int mouseX = LOWORD(lParam);
    const int mouseY = HIWORD(lParam);

    const int localX = mouseX - x - GRID_LEFT;
    const int localY = mouseY - y - 50;

    const int gridX = localX / CELL_WIDTH;
    const int gridY = ROWS - 1 - localY / CELL_HEIGHT;

    if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
        AddNoteAt(gridX, gridY);
    }

    const RECT rect = { mouseX - CELL_WIDTH * 16, mouseY - CELL_HEIGHT, mouseX + CELL_WIDTH * 16, mouseY + CELL_HEIGHT};

    InvalidateRect(hwnd, &rect, TRUE);
}

void PianoRoll::OnRButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int mouseX = LOWORD(lParam);
    const int mouseY = HIWORD(lParam);

    const int localX = mouseX - x - GRID_LEFT;
    const int localY = mouseY - y - 50;

    const int gridX = localX / CELL_WIDTH;
    const int gridY = ROWS - 1 - localY / CELL_HEIGHT;

    if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
        RemoveNoteAt(gridX, gridY);
    }

    const RECT rect = { mouseX - CELL_WIDTH * 16, mouseY - CELL_HEIGHT, mouseX + CELL_WIDTH * 16, mouseY + CELL_HEIGHT };

    InvalidateRect(hwnd, &rect, TRUE);
}

void PianoRoll::OnPaint(HDC hdc) {    
    DrawKeyboard(hdc);
    DrawGrid(hdc);
    DrawNotes(hdc);
}

void PianoRoll::DrawKeyboard(HDC hdc) {
    HBRUSH whiteBrush = CreateSolidBrush(WHITE_KEY_COLOR);
    HBRUSH blackBrush = CreateSolidBrush(BLACK_KEY_COLOR);

    for (int i = 0; i < ROWS; ++i) {
        RECT keyRect = { x, y + 50 + i * CELL_HEIGHT, x + KEYBOARD_WIDTH, y + 50 + (i + 1) * CELL_HEIGHT };

        if (IsBlackKey(ROWS - i - 1)) {
            FillRect(hdc, &keyRect, blackBrush);
        }
        else {
            FillRect(hdc, &keyRect, whiteBrush);
        }
    }

    DeleteObject(whiteBrush);
    DeleteObject(blackBrush);
}

void PianoRoll::DrawGrid(HDC hdc) {
    HPEN gridPen = CreatePen(PS_SOLID, 1, NOTE_LINE_COLOR);
    HPEN measurePen = CreatePen(PS_SOLID, 2, MEASURE_COLOR);
    HBRUSH stripBrush = CreateSolidBrush(BLACK_NOTE_STRIP_COLOR);

    SelectObject(hdc, gridPen);

    for (int i = 0; i <= ROWS; ++i) {
        if (IsBlackKey(ROWS - i - 1)) {
            RECT stripRect = { x + GRID_LEFT, y + 50 + i * CELL_HEIGHT, x + GRID_LEFT + COLS * CELL_WIDTH, y + 50 + (i + 1) * CELL_HEIGHT };
            FillRect(hdc, &stripRect, stripBrush);
        }

        MoveToEx(hdc, x + GRID_LEFT, y + 50 + i * CELL_HEIGHT, NULL);
        LineTo(hdc, x + GRID_LEFT + COLS * CELL_WIDTH, y + 50 + i * CELL_HEIGHT);
    }

    for (int j = 0; j <= COLS; ++j) {
        SelectObject(hdc, (j % MEASURE_DIVISION == 0) ? measurePen : gridPen);

        MoveToEx(hdc, x + GRID_LEFT + j * CELL_WIDTH, y + 50, NULL);
        LineTo(hdc, x + GRID_LEFT + j * CELL_WIDTH, y + 50 + ROWS * CELL_HEIGHT);
    }

    DeleteObject(gridPen);
    DeleteObject(measurePen);
    DeleteObject(stripBrush);
}

void PianoRoll::DrawNotes(HDC hdc) {
    HBRUSH noteBrush = CreateSolidBrush(NOTE_COLOR);
    HPEN notePen = CreatePen(PS_SOLID, 1, NOTE_BORDER_COLOR);

    SelectObject(hdc, noteBrush);
    SelectObject(hdc, notePen);

    for (const Note& note : notes) {
        RECT noteRect = {
            x + GRID_LEFT + note.x * CELL_WIDTH,
            y + 50 + (ROWS - note.y) * CELL_HEIGHT,
            x + GRID_LEFT + (note.x + note.length) * CELL_WIDTH,
            y + 50 + (ROWS - note.y - 1) * CELL_HEIGHT
        };

        Rectangle(hdc, noteRect.left, noteRect.top, noteRect.right, noteRect.bottom);
    }

    DeleteObject(noteBrush);
    DeleteObject(notePen);
}

void PianoRoll::AddNoteAt(int x, int y) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;

    const int maxLength = std::min(currentDuration, COLS - x);

    const auto it = std::find_if(notes.begin(), notes.end(), [x, y](const Note& note) {
        return note.y == y && x >= note.x && x < note.x + note.length;
        });

    if (it == notes.end()) {
        notes.push_back({ x, y, maxLength });
    }
}

void PianoRoll::RemoveNoteAt(int x, int y) {
    notes.erase(std::remove_if(notes.begin(), notes.end(), [x, y](const Note& note) {
        return note.y == y && x >= note.x && x < note.x + note.length;
        }), notes.end());
}

bool PianoRoll::IsBlackKey(int midiNote) {
    static const int blackKeys[] = { 1, 3, 6, 8, 10 };
    return std::find(std::begin(blackKeys), std::end(blackKeys), midiNote % 12) != std::end(blackKeys);
}