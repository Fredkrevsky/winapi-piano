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

    unordered_set<int> activeKeys;

    void SoundWorker();
    void loadDefaultSounds();
};

