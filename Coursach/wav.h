#pragma once
#define NOMINMAX
#include <windows.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
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

class WavSound {
public:
    WavSound();
    void setSize(int size);
    void addToBuffer(const WavSound& sound, int start);
    void loadFromWav(const wstring& path);
    int getSize() const;
    const short* data() const;
    void limiter(float velocity);

private:
    int size{};
    vector<short> buffer{};
    bool checkWav(const WAVHEADER& header);
};

bool SaveWavToFile(const std::string& filename, const void* wavData, size_t wavSize);

std::wstring openFileDialog(HWND hWnd);

bool fileExists(wstring& path);

wstring getFileName(wstring& path);