#ifndef NIFTIREADER_H
#define NIFTIREADER_H

#include <vector>
#include <string>
#include <ios>
#include <fstream>
#include <cstring>

using namespace std;

// Nifti 檔案讀取器
class niftiReader {
    public:
        // 空的建構子與解構子
        niftiReader() {};
        ~niftiReader() {};

        // 根據指定的 nifti 檔案路徑，讀取該 nifti 檔案為 bytes 向量
        void readNiftiAsBytes(const string niftiFilePath, vector<char> &outputBytesVector);

        // 根據 nifit 檔案的 bytes 向量，取得標頭大小
        int getHeaderSize(const vector<char> &bytesVector);

        // 根據 nifit 檔案的 bytes 向量，取得 Data array dimensions
        void getDimension(const vector<char> &bytesVector, vector<short> &dim);

        // 根據 nifit 檔案的 bytes 向量，取得 Number of bits per voxel.
        short getBitpix(const vector<char> &bytesVector);

        // 根據 nifit 檔案的 bytes 向量，取得 Data type
        short getDataType(const vector<char> &bytesVector);
};

#endif