#include "KeyboardPiano.h"
#include <algorithm>
#include <string>

using std::lock_guard, std::unique_lock;

KeyboardPiano::KeyboardPiano() : currentNote{ -1 }, stopThread(false), hasNewNote(false), channels(64) {
    FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, nullptr);
    loadDefaultSounds();
    system->setDSPBufferSize(64, 4);
}

KeyboardPiano::~KeyboardPiano() {
    stop();
    for (auto& octave : piano) {
        for (auto* note : octave) {
            note->release();
        }
    }
    system->close();
    system->release();
}

void KeyboardPiano::loadDefaultSounds() {
    std::ranges::for_each(piano, [&, octave = 3](auto& row) mutable {
        std::ranges::for_each(row, [&, note = 0](auto& elem) mutable {
            std::string path = "sounds\\piano\\" + notes[note] + std::to_string(octave) + ".wav";
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
        std::lock_guard<std::mutex> lock(mtx);
        stopThread = true;
        cv.notify_one();
    }
    if (soundThread.joinable()) {
        soundThread.join();
    }
}

void KeyboardPiano::playNote(int note) {
    lock_guard<std::mutex> lock(mtx);
    currentNote = note;
    hasNewNote = true;
    cv.notify_one();
}

void KeyboardPiano::OnKeyDown(WPARAM wParam) {
    int note = static_cast<int>(wParam);
    if (!activeKeys.contains(note)) {
        activeKeys.insert(note);
        if (keyToNote.contains(wParam)) {
            playNote(keyToNote[wParam]);
        }
    }
}

void KeyboardPiano::OnKeyUp(WPARAM wParam) {
    int note = static_cast<int>(wParam);
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
            int t = currentNote / 12;
            int i = currentNote % 12;
            system->playSound(piano[t][i], nullptr, false, &channels[t * 12 + i]);
            system->update();
        }

        hasNewNote = false;
    }
}