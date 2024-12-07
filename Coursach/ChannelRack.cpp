#include "ChannelRack.h"
#include <windowsx.h>
#include <algorithm>
#include <array>

using std::fill, std::array;

ChannelRack::ChannelRack(HWND hwnd, int x = 0, int y = 0) :
    numTracks(4),
    numSteps(32),
    posx(x),
    posy(y),
    buttonStates(numTracks, vector<bool>(numSteps, false)) {
    
    createButtons(hwnd);
    
    lightBrush = CreateSolidBrush(RGB(200, 206, 226));
    pinkBrush = CreateSolidBrush(RGB(240, 195, 197));
    greyBrush = CreateSolidBrush(RGB(56, 62, 66));
    redBrush = CreateSolidBrush(RGB(107, 90, 91));
}

void ChannelRack::createButtons(HWND hwnd) {
    constexpr array<const LPCWSTR, 4> names = { L"Kick", L"Clap", L"Snare", L"Hat" };
    constexpr array<const LPCWSTR, 4> extraNames = { L"All", L"1/2", L"1/4", L"1/8" };

    for (int track = 0; track < numTracks; ++track) {
        vector<unique_ptr<Button>> trackButtons;

        for (int step = 0; step < numSteps; ++step) {
            int x = posx + step * 20;
            int y = posy + 5 + track * 50;

            unique_ptr<Button> btn{ Button::createSmall(x, y, lastId++, hwnd) };
            trackButtons.emplace_back(std::move(btn));
        }

        for (int i = 0, size = extraNames.size(); i < size; ++i) {
            int x = posx + (numSteps + 2 + 4 * i) * 20;
            int y = posy + track * 50;

            unique_ptr<Button> extraBtn{ Button::createFreq(extraNames[i], x, y, lastId++, hwnd) };
            trackButtons.emplace_back(std::move(extraBtn));
        }
        buttons.emplace_back(std::move(trackButtons));
    }
}

ChannelRack::~ChannelRack() { }

bool ChannelRack::isClicked(WPARAM wParam, LPARAM lParam) {
    int id = static_cast<int>(wParam);
    return id >= CHANNEL_RACK_START_ID && id <= CHANNEL_RACK_END_ID;
}

void ChannelRack::toggleButton(int track, int step) {
    vector<bool>::reference ref = buttonStates[track][step];
    ref = !ref;
}

void ChannelRack::onClick(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    int btnId = static_cast<int>(wParam);
    auto [row, col] = getRowCol(btnId);
    if (col < 32) {
        toggleButton(row, col);
    }
    else {
        fillStatus(row, 1 << (col - 32));
    }
    auto [x, y, w, h] = getCoord(wParam);
    RECT rect = { x - 10, y - 10, x + w + 10, y + h + 10 };
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
    auto [row, col] = getRowCol(btnId);
    if (col < 32 && row < 4) {
        auto elem = buttons[row][col].get();
        auto [x, y] = elem->getPos();
        return { x, y, 15, 20};
    }
    else if (row < 4) {
        auto elem = buttons[row][0].get();
        auto [x, y] = elem->getPos();
        return { x, y, 635, 20 };
    }
    return {posx, posy, 635, 200};
}

void ChannelRack::FillButtonColor(WPARAM wParam, LPARAM lParam) {
    int id = static_cast<int>(wParam);
    if (id < startId || id > lastId) return;
    size_t index = (id - CHANNEL_RACK_START_ID);

    auto [row, col] = getRowCol(id);
    if (col >= 32) return;

    bool isSelected = buttonStates[row][col];
    bool mode = (col % 8 >= 4);

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
    int id = btnId - startId;
    int track = id / 36;
    int step = id % 36;
    return { track, step };
}