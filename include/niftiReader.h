#ifndef NIFTIREADER_H
#define NIFTIREADER_H

#include <vector>
#include <string>
#include <ios>
#include <fstream>

using namespace std;

// Nifti 檔案讀取器
class niftiReader {
    public:
        // 空的建構子與解構子
        niftiReader() {};
        ~niftiReader() {};

        // 根據指定的 nifti 檔案路徑，讀取該 nifti 檔案為 bytes 向量
        void readNiftiAsBytes(const string niftiFilePath, vector<char> &outputBytesVector);
};

#endif