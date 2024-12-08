#ifndef WAV_H
#define WAV_H

#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <commdlg.h> 

using namespace std;

typedef struct _WAVHEADER
{
    uint32_t        chunkId = 1179011410;
    uint32_t        chunkSize = 0;
    uint32_t        format = 1163280727;

    uint32_t        subchunk1Id = 544501094;
    uint32_t        subchunk1Size = 16;
    uint16_t        audioFormat = 1;
    uint16_t        numChannels = 2;
    uint32_t        sampleRate = 44100;
    uint32_t        byteRate = numChannels * sampleRate * 2;
    uint16_t        blockAlign = 4;
    uint16_t        bitsPerSample = 16;

    uint32_t        subchunk2ID = 1635017060;
    uint32_t        subchunk2Size = 0;
} WAVHEADER;

class WavSound final {
public:
    WavSound();
    void setSize(int size);
    void addToBuffer(const WavSound& sound, int start);
    void addToBuffer(const WavSound& sound, int start, int duration);
    void loadFromWav(const wstring& path);
    int getSize() const;
    constexpr const int* data() const;
    void saveToFile(const std::wstring& path);

    static wstring openFileDialog(HWND hWnd);
    static wstring saveFileDialog(HWND hWnd);
    static wstring getFileName(const wstring& path);
    static bool fileExists(const wstring& path);

private:
    vector<int> buffer{};
    vector<short> wavData{};

    bool checkWav(const WAVHEADER& header);
    void master();
};
#endif