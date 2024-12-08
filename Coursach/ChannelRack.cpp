#include "ChannelRack.h"
#include <windowsx.h>
#include <algorithm>
#include <array>

using std::fill, std::array;

ChannelRack::ChannelRack(HWND hwnd, int x, int y)
    : posx(x),
    posy(y),
    buttonStates(numTracks, vector<bool>(numSteps, false)) {

    buttons.resize(numTracks);
    std::ranges::for_each(buttons, [&](auto& buttonRow) {
        buttonRow.resize(numSteps + 4);
    });
    createButtons(hwnd);

    lightBrush = CreateSolidBrush(RGB(200, 206, 226));
    pinkBrush = CreateSolidBrush(RGB(240, 195, 197));
    greyBrush = CreateSolidBrush(RGB(56, 62, 66));
    redBrush = CreateSolidBrush(RGB(107, 90, 91));
}

void ChannelRack::createButtons(HWND hwnd) {
    constexpr std::array<const LPCWSTR, 4> extraNames = { L"All", L"1/2", L"1/4", L"1/8" };

    std::ranges::for_each(buttons, [&, track = 0](auto& buttonRow) mutable {
        std::ranges::for_each(buttonRow, [&, step = 0](auto& button) mutable {
            if (step < numSteps) {
                const int x = posx + step * 20;
                const int y = posy + 5 + track * 50;
                button.reset(Button::createSmall(x, y, lastId++, hwnd));
            }
            else {
                const int x = posx + (step + 3 * (step - numSteps)) * 20 + 40;
                const int y = posy + track * 50;
                button.reset(Button::createFreq(extraNames[step - numSteps], x, y, lastId++, hwnd));
            }
            ++step;
            });
        ++track;
        });
}

ChannelRack::~ChannelRack() { }

bool ChannelRack::isClicked(WPARAM wParam, LPARAM lParam) {
    const int id = static_cast<int>(wParam);
    return id >= CHANNEL_RACK_START_ID && id <= CHANNEL_RACK_END_ID;
}

void ChannelRack::toggleButton(int track, int step) {
    vector<bool>::reference ref = buttonStates[track][step];
    ref = !ref;
}

void ChannelRack::onClick(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int btnId = static_cast<int>(wParam);
    const auto [row, col] = getRowCol(btnId);
    if (col < 32) {
        toggleButton(row, col);
    }
    else {
        fillStatus(row, 1 << (col - 32));
    }
    const auto [x, y, w, h] = getCoord(wParam);
    const RECT rect = { x - 10, y - 10, x + w + 10, y + h + 10 };
    InvalidateRect(hwnd, &rect, TRUE);
}

void ChannelRack::fillStatus(int row, int freq) {
    auto& vec = buttonStates[row];
    vector<bool> status(32, false);
    bool flag = false;

    for (int i = 0; i < 32; i += freq) {
        if (!vec[i]) flag = true;
        status[i] = true;
    }

    if (flag) vec = status;
    else fill(vec.begin(), vec.end(), false);
}

tuple<int, int, int, int> ChannelRack::getCoord(int btnId) {
    const auto [row, col] = getRowCol(btnId);
    if (col < 32 && row < 4) {
        const auto elem = buttons[row][col].get();
        const auto [x, y] = elem->getPos();
        return { x, y, 15, 20};
    }
    else if (row < 4) {
        const auto elem = buttons[row][0].get();
        const auto [x, y] = elem->getPos();
        return { x, y, 635, 20 };
    }
    return {posx, posy, 635, 200};
}

void ChannelRack::FillButtonColor(WPARAM wParam, LPARAM lParam) {
    const int id = static_cast<int>(wParam);
    if (id < startId || id > lastId) return;
    const size_t index = (id - CHANNEL_RACK_START_ID);

    const auto [row, col] = getRowCol(id);
    if (col >= 32) return;

    const bool isSelected = buttonStates[row][col];
    const bool mode = (col % 8 >= 4);

    HBRUSH brush = nullptr;
    if (isSelected && mode)
        brush = pinkBrush;
    else if (isSelected)
        brush = lightBrush;
    else if (mode)
        brush = redBrush;
    else
        brush = greyBrush;

    LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
    HDC hdc = pDIS->hDC;
    RECT rect = pDIS->rcItem;

    FillRect(hdc, &rect, brush);
    FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
}

vector<vector<bool>> ChannelRack::getData() {
    return buttonStates;
}

pair<int, int> ChannelRack::getRowCol(int btnId) {
    const int id = btnId - startId;
    const int track = id / 36;
    const int step = id % 36;
    return { track, step };
}