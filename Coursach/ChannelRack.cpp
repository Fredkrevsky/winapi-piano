#include "ChannelRack.h"
#include <windowsx.h>
#include <algorithm>

using std::fill;

//PatternController::Pattern::Pattern()
//    : buttonStates(4, std::vector<bool>(32, false)) { }
//
//void PatternController::Pattern::clear() {
//    std::ranges::for_each(buttonStates, [](auto& row) {
//        std::fill(row.begin(), row.end(), false);
//        });
//}
//
//PatternController::PatternController(HWND hwnd, int x, int y) {
//    patterns.push_back({ 0, {} });
//    hComboBox = CreateWindowW(L"COMBOBOX", nullptr,
//        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
//        x, y, 100, 200,
//        hwnd, nullptr, nullptr, nullptr);
//
//    const std::array<LPCWSTR, 3> captions = { { L"Pattern 1", L"Add pattern", L"Remove pattern" } };
//    for (const auto& caption : captions) {
//        SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)caption);
//    }
//    SendMessageW(hComboBox, CB_SETCURSEL, 0, 0);
//}
//
//PatternController::~PatternController() { }
//
//bool PatternController::isClicked(WPARAM wParam, LPARAM lParam) {
//    return (HWND)lParam == hComboBox && HIWORD(wParam) == CBN_SELCHANGE;
//}
//
//void PatternController::onClick() {
//    int result = SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);
//    if (result == size) {
//        if (size < 6) {
//            currentPattern = size++;
//            patterns.push_back({ last++, {} });
//        }
//    }
//    else if (result == size + 1) {
//        if (size > 1) {
//            patterns.erase(std::next(patterns.begin(), currentPattern));
//            size--;
//        }
//        else {
//            patterns.begin()->second.clear();
//        }
//        currentPattern = 0;
//    }
//    else {
//        currentPattern = result;
//    }
//
//    SendMessageW(hComboBox, CB_RESETCONTENT, 0, 0);
//    for (auto it = patterns.begin(); it != patterns.end(); ++it) {
//        std::wstring caption = L"Pattern " + std::to_wstring(it->first + 1);
//        SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)caption.c_str());
//    }
//    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Add pattern");
//    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Remove pattern");
//    SendMessageW(hComboBox, CB_SETCURSEL, currentPattern, 0);
//}
//
//void PatternController::setState(std::vector<std::vector<bool>>&& newStatus) {
//    patterns[currentPattern].second.buttonStates = std::move(newStatus);
//}
//
//std::vector<std::vector<bool>> PatternController::getState() {
//    return patterns[currentPattern].second.buttonStates;
//}


ChannelRack::ChannelRack(HWND hwnd, int x = 0, int y = 0) :
    numTracks(4),
    numSteps(32),
    posx(x),
    posy(y),
    buttonStates(numTracks, vector<bool>(numSteps, false))/*,
    patternController(hwnd, 50, 100)*/ {
    
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
    return id >= CHANNEL_RACK_START_ID && id <= CHANNEL_RACK_END_ID/* || patternController.isClicked(wParam, lParam)*/;
}

void ChannelRack::toggleButton(int track, int step) {
    vector<bool>::reference ref = buttonStates[track][step];
    ref = !ref;
}

void ChannelRack::onClick(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    /*if (patternController.isClicked(wParam, lParam)) {
        patternController.setState(std::move(buttonStates));
        patternController.onClick();
        buttonStates = patternController.getState();
    }
    else {*/
        int btnId = static_cast<int>(wParam);
        auto [row, col] = getRowCol(btnId);
        if (col < 32) {
            toggleButton(row, col);
        }
        else {
            fillStatus(row, 1 << (col - 32));
        }
    //}
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