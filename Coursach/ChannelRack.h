#ifndef CHANNEL_RACK_H
#define CHANNEL_RACK_H

#define NOMINMAX
#include <windows.h>
#include "Controls.h"
#include <vector>
#include <memory>

#define CHANNEL_RACK_START_ID 1000
#define CHANNEL_RACK_END_ID 1999

using std::vector, std::tuple, std::unique_ptr;

class ChannelRack final {
public:
    ChannelRack(HWND hwnd, int x, int y);

    ~ChannelRack();

    void toggleButton(int track, int step);

    bool isClicked(WPARAM wParam, LPARAM lParam);

    void onClick(HWND hwnd, WPARAM wParam, LPARAM lParam);

    void fillStatus(int row, int freq);

    vector<vector<bool>> getData();

    void FillButtonColor(WPARAM wParam, LPARAM lParam);

private:
    tuple<int, int, int, int> getCoord(int btnId);

    pair<int, int> getRowCol(int btnId);
    
    void createButtons(HWND hwnd);

    const int numTracks{ 4 };
    const int numSteps{ 32 };
    const int posx, posy;

    vector<vector<bool>> buttonStates;
    HBRUSH lightBrush, pinkBrush, greyBrush, redBrush;
    vector<vector<unique_ptr<Button>>> buttons;

    const int startId{ CHANNEL_RACK_START_ID };
    int lastId{ CHANNEL_RACK_START_ID };

    const int swidth{ 15 };
    const int sheight{ 20 };
};

#endif