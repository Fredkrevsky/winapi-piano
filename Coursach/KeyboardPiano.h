#pragma once
#include <windows.h>
#include <vector>
#include <array>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include "core/inc/fmod.hpp"

using std::vector, std::mutex, std::condition_variable, std::thread, std::unordered_set;
using std::unordered_map, std::array, std::string;
using FMOD::Channel, FMOD::Sound, FMOD::System;

class KeyboardPiano {
public:
    KeyboardPiano();
    ~KeyboardPiano();

    void start();
    void stop();
    void OnKeyDown(WPARAM wParam);
    void OnKeyUp(WPARAM wParam);
    void playNote(int note);

private:
    int currentNote;
    bool stopThread;
    bool hasNewNote;

    mutex mtx;
    condition_variable cv;
    thread soundThread;

    System* system;
    vector<Channel*> channels;
    unordered_set<int> channelStatus;
    array<array<Sound*, 12>, 3> piano;

    const array<string, 12> notes = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    unordered_set<int> activeKeys;
    unordered_map<WPARAM, int> keyToNote = {
    {'Z', 0},  {'S', 1},  {'X', 2},  {'D', 3},  {'C', 4},
    {'V', 5},  {'G', 6},  {'B', 7},  {'H', 8},  {'N', 9},
    {'J', 10}, {'M', 11}, {VK_OEM_COMMA, 12}, {'L', 13}, {VK_OEM_PERIOD, 14},
    {VK_OEM_1, 15}, {VK_OEM_2, 16},
    {'Q', 12}, {'2', 13}, {'W', 14},
    {'3', 15}, {'E', 16}, {'R', 17}, {'5', 18}, {'T', 19},
    {'6', 20}, {'Y', 21}, {'7', 22}, {'U', 23}, {'I', 24},
    {'9', 25}, {'O', 26}, {'0', 27}, {'P', 28}, {VK_OEM_4, 29},
    {VK_OEM_PLUS, 30}, {VK_OEM_6, 31}
    };

    void SoundWorker();
    void loadDefaultSounds();
};

