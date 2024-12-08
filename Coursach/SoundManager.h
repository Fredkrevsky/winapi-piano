#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#define NOMINMAX
#include <windows.h>
#include <vector>
#include <string>
#include <array>
#include "Controls.h"
#include "wav.h"
#include "core/inc/fmod.hpp"
#include "pianoRoll.h"

using FMOD::Sound;
using std::vector, std::array;

class SoundManager final {
public:
    SoundManager(HWND hwnd, int x, int y);
    ~SoundManager();

    void play();
    void stop();
    void saveToFile(const wstring& path);
    void master(const vector<vector<bool>>& drumData, const vector<Note>& pianoData);
    void setBpm(const int _bpm);
    bool isClicked(WPARAM wParam, LPARAM lParam);
    void onClick(HWND hwnd, WPARAM wParam, LPARAM lParam);
private:
    void loadDefaultSounds();
    void createButtons(HWND hwnd, int x, int y);

    int bpm{ 100 };
    const int tacts{ 2 };
    const int rate{ 44100 };

    vector<unique_ptr<Button>> sampleButtons;
    vector<WavSound> loadedSamples;
    WavSound masterSound;
    unique_ptr<char> wavData;
    array<array<WavSound, 12>, 4> piano;
};

#endif