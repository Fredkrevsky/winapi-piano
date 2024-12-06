#include "PianoRoll.h"

PianoRoll::PianoRoll(HWND hwnd, int _x, int _y) : x(_x), y(_y){
    int yPos = 50 + ROWS * CELL_HEIGHT + 20;

    btnWholeNote = CreateWindow(L"BUTTON", L"Whole Note", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 50, y + yPos, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    btnHalfNote = CreateWindow(L"BUTTON", L"Half Note", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 250, y + yPos, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    btnQuarterNote = CreateWindow(L"BUTTON", L"Quarter Note", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 450, y + yPos, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    btnEighthNote = CreateWindow(L"BUTTON", L"Eighth Note", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 650, y + yPos, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

    btnSixteenthNote = CreateWindow(L"BUTTON", L"Sixteenth Note", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + 850, y + yPos, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
}

vector<Note> PianoRoll::getData() {
    return notes;
}

bool PianoRoll::isClicked(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    int posx = LOWORD(lParam);
    int posy = HIWORD(lParam);

    int localX = posx - x - GRID_LEFT;
    int localY = posy - y - 50;

    int gridX = localX / CELL_WIDTH;
    int gridY = localY / CELL_HEIGHT;

    return gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS;
}

void PianoRoll::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    if ((HWND)lParam == btnWholeNote) {
        noteDuration = 16;
    }
    else if ((HWND)lParam == btnHalfNote) {
        noteDuration = 8;
    }
    else if ((HWND)lParam == btnQuarterNote) {
        noteDuration = 4;
    }
    else if ((HWND)lParam == btnEighthNote) {
        noteDuration = 2;
    }
    else if ((HWND)lParam == btnSixteenthNote) {
        noteDuration = 1;
    }
}

void PianoRoll::OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    int mouseX = LOWORD(lParam);
    int mouseY = HIWORD(lParam);

    int localX = mouseX - x - GRID_LEFT;
    int localY = mouseY - y - 50;

    int gridX = localX / CELL_WIDTH;
    int gridY = ROWS - 1 - localY / CELL_HEIGHT;

    if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
        AddNoteAt(gridX, gridY);
    }

    RECT rect = { mouseX - CELL_WIDTH * 16, mouseY - CELL_HEIGHT, mouseX + CELL_WIDTH * 16, mouseY + CELL_HEIGHT};

    InvalidateRect(hwnd, &rect, TRUE);
}

void PianoRoll::OnRButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    int mouseX = LOWORD(lParam);
    int mouseY = HIWORD(lParam);

    int localX = mouseX - x - GRID_LEFT;
    int localY = mouseY - y - 50;

    int gridX = localX / CELL_WIDTH;
    int gridY = ROWS - 1 - localY / CELL_HEIGHT;

    if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) {
        RemoveNoteAt(gridX, gridY);
    }

    RECT rect = { mouseX - CELL_WIDTH * 16, mouseY - CELL_HEIGHT, mouseX + CELL_WIDTH * 16, mouseY + CELL_HEIGHT };

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

    int maxLength = std::min(noteDuration, COLS - x);

    auto it = std::find_if(notes.begin(), notes.end(), [x, y](const Note& note) {
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