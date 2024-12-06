#include "SoundManager.h"

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

void SoundManager::createButtons(HWND hwnd, int x, int y) {
    int lastId{ SOUND_MANAGER_START_ID };
    int track{ 0 };
    for (int track = 0; track < 4; ++track) {
        unique_ptr<Button> uptr{ Button::createSimple(L"", x, y + 50 * track, lastId++, hwnd) };
        sampleButtons.emplace_back(std::move(uptr));
    }
}

void SoundManager::loadDefaultSounds() {
    for (int t = 0; t < 4; ++t) {
        for (int i = 0; i < 12; ++i) {
            string note(notes[i]);
            wstring path = DEFAULT_PIANO_PATH + wstring(note.begin(), note.end()) + to_wstring(t + 1) + L".wav";
            piano[t][i].loadFromWav(path.c_str());
            piano[t][i].limiter(0.1f);
        }
    }

    const vector<wstring> defaultPaths = {
        DEFAULT_KICK_PATH,
        DEFAULT_CLAP_PATH,
        DEFAULT_SNARE_PATH,
        DEFAULT_HAT_PATH
    };

    std::ranges::for_each(defaultPaths, [&, i=0](const auto& path) mutable {
        WavSound sound;
        wstring wpath(path.begin(), path.end());
        sound.loadFromWav(wpath);
        loadedSamples.emplace_back(sound);
        wstring name = getFileName(wpath);
        sampleButtons[i]->setText(name);
        i++;
    });
}

bool SoundManager::isClicked(WPARAM wParam, LPARAM lParam) {
    int id = static_cast<int>(wParam);
    return id >= SOUND_MANAGER_START_ID && id <= SOUND_MANAGER_END_ID;
}

void SoundManager::onClick(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    int index = static_cast<int>(wParam) - SOUND_MANAGER_START_ID;
    wstring path = openFileDialog(hwnd);
    if (!fileExists(path)) {
        throw std::exception("Incorrect file path");
    }
    wstring name = getFileName(path);
    loadedSamples[index].loadFromWav(path);
    sampleButtons[index]->setText(name);
}

void SoundManager::master(vector<vector<bool>>& drumData, vector<Note> pianoData) {

    int tactSize = rate * 60 * 2 * 4 / bpm;

    int SIZE = tactSize * tacts;
    masterSound.setSize(SIZE);

    for (size_t index = 0; index < 4; ++index) {
        auto& sample = loadedSamples[index];
        sample.limiter(0.1f);

        vector<bool>& vec = drumData[index];
        WavSound curr;
        curr.setSize(SIZE);

        for (int i = 0; i < 32; ++i) {
            if (vec[i])
                curr.addToBuffer(sample, i * tactSize / 16);
        }
        masterSound.addToBuffer(curr, 0);
    }
}

void SoundManager::setBpm(const int _bpm) {
    bpm = _bpm;
}

void SoundManager::play() {
    WAVHEADER header;

    header.subchunk2Size = masterSound.getSize() * 2;
    header.chunkSize = 36 + header.subchunk2Size;

    int wavSize = sizeof(WAVHEADER) + header.subchunk2Size;
    wavData.reset( new char[wavSize] );

    memcpy(wavData.get(), &header, sizeof(WAVHEADER));
    memcpy(wavData.get() + sizeof(WAVHEADER), masterSound.data(), header.subchunk2Size);

    SaveWavToFile("output.wav", wavData.get(), wavSize);

    PlaySound(reinterpret_cast<LPCWSTR>(wavData.get()),
        NULL, SND_MEMORY | SND_ASYNC | SND_LOOP);
}

void SoundManager::stop() {
    PlaySound(nullptr, nullptr, 0);
}
