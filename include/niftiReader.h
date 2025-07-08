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

        // 根據 nifit 檔案的 bytes 向量，取得起始的 Voxel offset
        float getVoxelOffset(const vector<char> &bytesVector);

        // 根據 nifit 檔案的 bytes 向量，以及 X, Y, Z 座標，取得對應的 Voxel 值
        // 注意：這個方法需要根據實際的數據類型和維度來實現
        // 這裡假設 Voxel 值是 short
        // 這裡的 voxelOffset 是從 getVoxelOffset() 方法取得的
        // 這裡的 dim 是從 getDimension() 方法取得的
        inline short getVoxelValue(
            const vector<char> &bytesVector, const vector<short> &dim, size_t voxelOffset,
            int x, int y, int z
        ) {
            // 檢查座標是否在有效範圍內
            if (x < 0 || x >= dim[1] || y < 0 || y >= dim[2] || z < 0 || z >= dim[3]) {
                throw out_of_range("Voxel coordinates are out of bounds.");
            }

            // 計算目標 Voxel 在 bytesVector 中的偏移量
            size_t offset = static_cast<size_t>(voxelOffset) + (x + y * dim[1] + z * dim[1] * dim[2]) * sizeof(short);

            // 檢查偏移量是否在有效範圍內
            if (offset >= bytesVector.size()) {
                throw out_of_range("Voxel offset is out of bounds.");
            }

            return *reinterpret_cast<const short*>(bytesVector.data() + offset); // 回傳 Voxel 值
        };
};

#endif