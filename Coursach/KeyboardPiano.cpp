#include "KeyboardPiano.h"
#include <algorithm>
#include <string>
#include <unordered_map>

using std::lock_guard, std::unique_lock, std::unordered_map, std::string, std::mutex;

KeyboardPiano::KeyboardPiano() : currentNote{ -1 }, stopThread(false), hasNewNote(false) {
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, nullptr);
    loadDefaultSounds();
    system->setDSPBufferSize(64, 4);
}

KeyboardPiano::~KeyboardPiano() {
    stop();
    std::ranges::for_each(piano, [](auto& octave) {
        std::ranges::for_each(octave, [](auto& note) {
            note->release();
        });
    });
    system->close();
    system->release();
}

void KeyboardPiano::loadDefaultSounds() {

    static const array<string, 12> notes = { 
        "C", "C#", "D", "D#", 
        "E", "F", "F#", "G", 
        "G#", "A", "A#", "B" 
    };

    std::ranges::for_each(piano, [&, octave = 3](auto& row) mutable {
        std::ranges::for_each(row, [&, note = 0](auto& elem) mutable {
            string path = "sounds\\piano\\" + notes[note] + std::to_string(octave) + ".wav";
            system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &elem);
            note++;
            });
        octave++;
        });
}

void KeyboardPiano::start() {
    soundThread = std::thread(&KeyboardPiano::SoundWorker, this);
}

void KeyboardPiano::stop() {
    {
        lock_guard<mutex> lock(mtx);
        stopThread = true;
        cv.notify_one();
    }
    if (soundThread.joinable()) {
        soundThread.join();
    }
}

void KeyboardPiano::playNote(int note) {
    lock_guard<mutex> lock(mtx);
    currentNote = note;
    hasNewNote = true;
    cv.notify_one();
}

void KeyboardPiano::OnKeyDown(WPARAM wParam) {

    static const unordered_map<WPARAM, int> keyToNote = {
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

    const int note = static_cast<int>(wParam);
    if (!activeKeys.contains(note)) {
        activeKeys.insert(note);
        if (auto it = keyToNote.find(wParam); it != keyToNote.end()) {
            playNote(it->second);
        }
    }
}

void KeyboardPiano::OnKeyUp(WPARAM wParam) {
    const int note = static_cast<int>(wParam);
    activeKeys.erase(note);
}

void KeyboardPiano::SoundWorker() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return hasNewNote || stopThread; });

        if (stopThread) {
            break;
        }

        if (currentNote != -1) {
            const int t = currentNote / 12;
            const int i = currentNote % 12;
            system->playSound(piano[t][i], nullptr, false, nullptr);
            system->update();
        }

        hasNewNote = false;
    }
}