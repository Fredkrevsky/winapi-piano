#pragma once
#define NOMINMAX
#include <windows.h>
#include "Controls.h"
#include <vector>
#include <memory>

using std::vector, std::tuple, std::unique_ptr;

class ChannelRack {
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

    int numTracks, numSteps;
    const int posx;
    const int posy;
    vector<vector<bool>> buttonStates;
    HBRUSH lightBrush, pinkBrush, greyBrush, redBrush;
    HWND hComboBox;


    vector<vector<unique_ptr<Button>>> buttons;

    int startId{ CHANNEL_RACK_START_ID };
    int lastId{ CHANNEL_RACK_START_ID };

    const int swidth{ 15 };
    const int sheight{ 20 };
};

