#include "SoundManager.h"
#include <algorithm>

#define DEFAULT_KICK_PATH L"sounds/kick.wav"
#define DEFAULT_CLAP_PATH L"sounds/clap.wav"
#define DEFAULT_SNARE_PATH L"sounds/snare.wav"
#define DEFAULT_HAT_PATH L"sounds/hat.wav"
#define DEFAULT_PIANO_PATH L"sounds/piano/"
#define SOUND_MANAGER_START_ID 2000
#define SOUND_MANAGER_END_ID   2005

SoundManager::SoundManager(HWND hwnd, int x = 0, int y = 0) {
    createButtons(hwnd, x, y);
    loadDefaultSounds();
}

SoundManager::~SoundManager() { }

void SoundManager::saveToFile(const wstring& path) {
    masterSound.saveToFile(path);
}

void SoundManager::createButtons(HWND hwnd, int x, int y) {
    int lastId{ SOUND_MANAGER_START_ID };
    int track{ 0 };
    for (int track = 0; track < 4; ++track) {
        unique_ptr<Button> uptr{ Button::createSimple(L"", x, y + 50 * track, lastId++, hwnd) };
        sampleButtons.emplace_back(std::move(uptr));
    }
}

void SoundManager::loadDefaultSounds() {
    static const array<string, 12> notes = {
        {"C", "C#", "D", "D#",
        "E", "F", "F#", "G",
        "G#", "A", "A#", "B"}
    };

    std::ranges::for_each(piano, [&, t=1](auto& row) mutable {
        std::ranges::for_each(row, [&, t, i=0](auto& elem) mutable {
            const auto& note = notes[i];
            const wstring path = DEFAULT_PIANO_PATH + wstring(note.begin(), note.end()) + to_wstring(t) + L".wav";
            elem.loadFromWav(path.c_str());
            i++;
            });
        t++;
        });

    static const array<LPCWSTR, 4> defaultPaths = {
        DEFAULT_KICK_PATH,
        DEFAULT_CLAP_PATH,
        DEFAULT_SNARE_PATH,
        DEFAULT_HAT_PATH
    };

    std::ranges::for_each(defaultPaths, [&, i=0](const auto& path) mutable {
        WavSound sound;
        sound.loadFromWav(path);
        loadedSamples.emplace_back(std::move(sound));
        wstring name = WavSound::getFileName(path);
        sampleButtons[i]->setText(name);
        i++;
    });
}

bool SoundManager::isClicked(WPARAM wParam, LPARAM lParam) {
    const int id = static_cast<int>(wParam);
    return id >= SOUND_MANAGER_START_ID && id <= SOUND_MANAGER_END_ID;
}

void SoundManager::onClick(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    const int index = static_cast<int>(wParam) - SOUND_MANAGER_START_ID;
    const wstring path = WavSound::openFileDialog(hwnd);
    loadedSamples[index].loadFromWav(path);

    const wstring name = WavSound::getFileName(path);
    sampleButtons[index]->setText(name);
}

void SoundManager::master(const vector<vector<bool>>& drumData, const vector<Note>& pianoData) {

    int tactSize = rate * 60 * 2 * 4 / bpm;

    int SIZE = tactSize * tacts;
    masterSound.setSize(SIZE);

    for (size_t index = 0; index < 4; ++index) {
        auto& sample = loadedSamples[index];

        const vector<bool>& vec = drumData[index];
        WavSound curr;
        curr.setSize(SIZE);

        for (int i = 0; i < 32; ++i) {
            if (vec[i])
                curr.addToBuffer(sample, i * tactSize / 16);
        }
        masterSound.addToBuffer(curr, 0);
    }

    for (const auto& elem : pianoData) {
        int start = elem.x * rate * 30 / bpm;
        int duration = elem.length * rate * 30 / bpm;
        int note = elem.y + 24;
        auto& sample = piano[note / 12][note % 12];
        masterSound.addToBuffer(sample, start, duration);
    }
}

void SoundManager::setBpm(const int _bpm) {
    bpm = _bpm;
}

void SoundManager::play() {
    masterSound.saveToFile(L"output.wav");

    PlaySound(L"output.wav",
        NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void SoundManager::stop() {
    PlaySound(nullptr, nullptr, 0);
}
