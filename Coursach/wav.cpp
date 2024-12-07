#include "wav.h"
#include <fstream>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <iterator>

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

    string pathStr(path.begin(), path.end());

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
    auto srcend = std::next(src, count);
    auto dest = std::next(buffer.begin(), start);
    
    std::transform(
        src, 
        srcend, 
        dest, 
        dest, 
        std::plus<int>()
    );
}

void WavSound::addToBuffer(const WavSound& sound, int start, int duration) {
    int count = min({ sound.getSize(), size - start, duration }) - 600;
    auto src = sound.data();
    auto srcend = std::next(src, count);
    auto dest = std::next(buffer.begin(), start);

    int DECAY = min(2000, count);
    auto decay_start_src = std::next(src, count - DECAY);
    auto decay_start_dest = std::next(dest, count - DECAY);

    std::transform(
        src,
        decay_start_src,
        dest,
        dest,
        std::plus<int>()
    );

    std::transform(
        decay_start_src,
        srcend,
        decay_start_dest,
        decay_start_dest,
        [=, velocity=DECAY] (const int src_val, const int dest_val) mutable {
            float k = static_cast<float>(velocity) / DECAY;
            int val = static_cast<int>(src_val * k);
            velocity--;
            return dest_val + val;
        }
    );

}

void WavSound::master() {
    float volume = 0.8f;

    auto [f, s] = std::minmax_element(buffer.begin(), buffer.end());
    int mn = *f, mx = *s;

    if (mn == 0 && mx == 0) return;

    float k = SHRT_MAX * volume / std::max(std::abs(mn), std::abs(mx));

    wavData = std::vector<short>(buffer.size());
    std::transform(
        buffer.begin(),
        buffer.end(),
        wavData.begin(),
        [k](int elem) { return static_cast<short>(elem * k); }
    );
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


