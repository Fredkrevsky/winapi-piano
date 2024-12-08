#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define NOMINMAX
#include <windows.h>
#include <memory>
#include "PianoRoll.h"
#include "ChannelRack.h"
#include "SoundManager.h"
#include "KeyboardPiano.h"
#include "PianoRoll.h"

class MainWindow final {
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow();

    bool InitInstance(int nCmdShow);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnCreate(HWND hwnd);
    void OnPaint(HWND hwnd);
    void OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnKeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnKeyUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void OnRButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
    void master();

    HWND hwndMain;
    std::unique_ptr<SoundManager> manager;
    std::unique_ptr<ChannelRack> channelRack;
    std::unique_ptr<BPMController> bpmController;
    std::unique_ptr<Button> buttonStart;
    std::unique_ptr<Button> buttonStop;
    std::unique_ptr<KeyboardPiano> keyboardPiano;
    std::unique_ptr<PianoRoll> pianoRoll;
    std::unique_ptr<Button> buttonSave;
};

#endif
