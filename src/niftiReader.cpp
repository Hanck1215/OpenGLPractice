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

// 根據 nifit 檔案的 bytes 向量，取得標頭大小 (先忽略大小端問題)
int niftiReader::getHeaderSize(const vector<char> &bytesVector) {
    int headerSize; // 預留 4 bytes 空間
    memcpy(&headerSize, bytesVector.data(), sizeof(headerSize)); // 複製 nifit 檔案的前 4 個 bytes 到 headerSize
    return headerSize; // 回傳 nifit 檔案的標頭大小
}

// 根據 nifit 檔案的 bytes 向量，取得 Data array dimensions
void niftiReader::getDimension(const vector<char> &bytesVector, vector<short> &dim) {
    // 確保 dim 向量有足夠的空間來存放維度數據
    dim.resize(8); // NIfTI-1 標準中，維度數據通常有 8 個 short

    // 從 bytesVector 中複製維度數據到 dim 向量
    memcpy(dim.data(), bytesVector.data() + 40, sizeof(short) * 8); // 假設維度數據從第 40 個 byte 開始
}

// 根據 nifit 檔案的 bytes 向量，取得 Number of bits per voxel.
short niftiReader::getBitpix(const vector<char> &bytesVector) {
    short bitpix; // 預留 2 bytes 空間
    memcpy(&bitpix, bytesVector.data() + 72, sizeof(bitpix)); // 複製 nifit 檔案的第 72 個 byte 開始的 4 個 bytes 到 bitpix
    return bitpix; // 回傳 Bitpix
}

// 根據 nifit 檔案的 bytes 向量，取得 Data type
short niftiReader::getDataType(const vector<char> &bytesVector) {
    short dataType; // 預留 2 bytes 空間
    memcpy(&dataType, bytesVector.data() + 70, sizeof(dataType)); // 複製 nifit 檔案的第 70 個 byte 開始的 2 個 bytes 到 dataType
    return dataType; // 回傳 Data type
}

// 根據 nifit 檔案的 bytes 向量，取得起始的 Voxel offset
float niftiReader::getVoxelOffset(const vector<char> &bytesVector) {
    float voxelOffset; // 預留 4 bytes 空間
    memcpy(&voxelOffset, bytesVector.data() + 108, sizeof(voxelOffset)); // 複製 nifit 檔案的第 108 個 byte 開始的 4 個 bytes 到 voxelOffset
    return voxelOffset; // 回傳 Voxel offset
}