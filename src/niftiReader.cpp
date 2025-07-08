#include "niftiReader.h"

// 根據指定的 nifti 檔案路徑，讀取該 nifti 檔案為 bytes 向量
void niftiReader::readNiftiAsBytes(const string niftiFilePath, vector<char> &outputBytesVector) {
    // 以二進位模式讀取檔案
    ifstream niftiFile(niftiFilePath, ios_base::binary);
    
    // 計算檔案有幾 bytes
    niftiFile.seekg(0, ios_base::end);
    size_t length = niftiFile.tellg();
    niftiFile.seekg(0, ios_base::beg);
    
    // 預留容量空間
    outputBytesVector.reserve(length);

    // 把檔案數據複製到 outputBytesVector
    copy(
        istreambuf_iterator<char>(niftiFile),
        istreambuf_iterator<char>(),
        back_inserter(outputBytesVector)
    );
}