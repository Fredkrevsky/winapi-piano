#pragma once
#define NOMINMAX
#include <windows.h>
#include "Controls.h"
#include <vector>

//class PatternController {
//    struct Pattern {
//        Pattern();
//        std::vector<std::vector<bool>> buttonStates;
//        void clear();
//    };
//
//public:
//    PatternController(HWND hwnd, int x, int y);
//    ~PatternController();
//
//    bool isClicked(WPARAM wParam, LPARAM lParam);
//    void onClick();
//    void setState(std::vector<std::vector<bool>>&& newStatus);
//    std::vector<std::vector<bool>> getState();
//
//private:
//    std::vector<std::pair<int, Pattern>> patterns;
//    int last{ 1 };
//    int size{ 1 };
//    int currentPattern{ 0 };
//    HWND hComboBox;
//};


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

