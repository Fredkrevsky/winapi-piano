#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "wav.h"
#include <fstream>
#include <algorithm>
#include <locale>
#include <codecvt>

using namespace std;

WavSound::WavSound() { }

bool WavSound::checkWav(const WAVHEADER& header) {
    return (header.chunkId == 1179011410 &&
        header.format == 1163280727);
}

void WavSound::loadFromWav(const wstring& path) {

    if (!fileExists(path)) {
        throw std::exception("Incorrect file path");
    }
    wstring name = getFileName(path);

    size = 0;
    WAVHEADER wavHeader;
    FILE* wavFile;

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string pathStr = converter.to_bytes(path);

    fopen_s(&wavFile, pathStr.c_str(), "rb");

    if (wavFile == nullptr) return;

    if (int bytesRead = fread(&wavHeader, 1, sizeof(WAVHEADER), wavFile))
    {
        if (!checkWav(wavHeader)) {
            fclose(wavFile);
            return;
        }
        int newSize = wavHeader.chunkSize / 2;
        size = newSize;

        vector<short> shortBuffer(newSize);
        size = fread(shortBuffer.data(), sizeof(short), size, wavFile);
        shortBuffer.resize(size);

        buffer.assign(shortBuffer.begin(), shortBuffer.end());
    }
    fclose(wavFile);
}

const int* WavSound::data() const {
    return buffer.data();
}

void WavSound::setSize(int _size) {
    buffer.clear();
    buffer.resize(_size, 0);
    size = _size;
}

int WavSound::getSize() const {
    return size;
}

void WavSound::addToBuffer(const WavSound& sound, int start) {
    int count = min(sound.getSize(), size - start) - 200;
    auto src = sound.data();
    auto dest = std::next(buffer.begin(), start);
    auto destend = std::next(dest, count);

    for (int i = 0; i < count; ++i, ++dest) {
        *dest += src[i];
    }
}

void WavSound::addToBuffer(const WavSound& sound, int start, int duration) {
    int count = min({ sound.getSize(), size - start, duration}) - 600;
    auto src = sound.data();
    auto dest = std::next(buffer.begin(), start);
    auto destend = std::next(dest, count);

    int DECAY = min(2000, count);

    for (int i = 0; i < count - DECAY; ++i, ++dest) {
        *dest += src[i];
    }
    for (int i = count - DECAY; i < count; ++i, ++dest) {
        int prev = src[i];
        float k = (count - i) / DECAY;
        int val = static_cast<int>(prev * k);
        *dest += val;
    }
}

void WavSound::master() {
    float volume = 0.8f;

    auto [f, s] = minmax_element(buffer.begin(), buffer.end());
    int mn = *f, mx = *s;

    if (mn == 0 && mx == 0) return;
    float k = SHRT_MAX * volume / max(abs(mn), abs(mx));

    wavData.clear();
    std::transform(buffer.begin(), buffer.end(), back_inserter(wavData), [=](const auto& elem) {
        return elem * k;
    });
}

void WavSound::saveToFile(const wstring& filename) {
    master();

    WAVHEADER header;

    header.subchunk2Size = size * sizeof(short);
    header.chunkSize = 36 + header.subchunk2Size;

    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&header), sizeof(WAVHEADER));
    outFile.write(reinterpret_cast<const char*>(wavData.data()), header.subchunk2Size);
    outFile.close();
}

wstring WavSound::openFileDialog(HWND hWnd) {
    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Wave Files\0*.wav\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return L"";
}

bool WavSound::fileExists(const wstring& path) {
    DWORD fileAttr = GetFileAttributesW(path.c_str());
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
}

wstring WavSound::getFileName(const wstring& path)
{
    size_t lastSlashPos = path.find_last_of(L"\\/");
    wstring fileName = (lastSlashPos == wstring::npos) 
        ? path 
        : path.substr(lastSlashPos + 1);

    size_t lastDotPos = fileName.find_last_of(L'.');
    if (lastDotPos != wstring::npos) {
        fileName = fileName.substr(0, lastDotPos);
    }

    return fileName;
}


