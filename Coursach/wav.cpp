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
    size = 0;
    WAVHEADER wavHeader;
    FILE* wavFile;

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string pathStr = converter.to_bytes(path);

    fopen_s(&wavFile, pathStr.c_str(), "rb");

    if (wavFile == nullptr) return;

    if (int bytesRead = fread(&wavHeader, 1, sizeof(WAVHEADER), wavFile))
    {
        if (!checkWav(wavHeader)) return;
        int newSize = wavHeader.chunkSize / 2;
        size = newSize;
        buffer.resize(newSize);
        size = fread(buffer.data(), 2, size, wavFile);
    }
    fclose(wavFile);
}

void WavSound::limiter(float limit) {
    auto [f, s] = minmax_element(buffer.begin(), buffer.end());
    short mn = *f, mx = *s;
    if (mn == 0 && mx == 0) return;
    float k = SHRT_MAX * limit / max(abs(mn), abs(mx));
    if (k < 1) {
        std::ranges::for_each(buffer, [=](auto& elem) {
            return static_cast<short>(elem * k);
            });
    }
}

const short* WavSound::data() const {
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
    int count = min(sound.getSize(), size - start);
    auto src = sound.data();
    auto dest = std::next(buffer.begin(), start);
    auto destend = std::next(dest, count - 200);

    for (int i = 0; i < count - 200; ++i, ++dest) {
        int val = src[i] + *dest;
        *dest = max(min(val, SHRT_MAX), SHRT_MIN);
    }
}

bool SaveWavToFile(const std::string& filename, const void* wavData, size_t wavSize) {
    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(wavData), wavSize);
    outFile.close();
    return TRUE;
}

std::wstring openFileDialog(HWND hWnd) {
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

bool fileExists(wstring& path) {
    DWORD fileAttr = GetFileAttributesW(path.c_str());
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
}

wstring getFileName(wstring& path)
{
    size_t lastSlashPos = path.find_last_of(L"\\/");
    std::wstring fileName = (lastSlashPos == std::wstring::npos) 
        ? path 
        : path.substr(lastSlashPos + 1);

    size_t lastDotPos = fileName.find_last_of(L'.');
    if (lastDotPos != std::wstring::npos) {
        fileName = fileName.substr(0, lastDotPos);
    }

    return fileName;
}


