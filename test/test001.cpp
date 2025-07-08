#include "niftiReader.h"
#include <cstdio>
#include <iostream>
#include <chrono> // 提供高精度時間測量功能
using namespace std;

int main(int argc, char **argv) {
    const string path = argv[1];
    vector<char> outputBytesVector;
    niftiReader niiReader;
    niiReader.readNiftiAsBytes(path, outputBytesVector);
    printf("header size: %d\n", niiReader.getHeaderSize(outputBytesVector));

    vector<short> dim;
    niiReader.getDimension(outputBytesVector, dim);
    printf("dim: %d, %d, %d, %d, %d, %d, %d, %d\n",
           dim[0], dim[1], dim[2], dim[3],
           dim[4], dim[5], dim[6], dim[7]);

    printf("number of bits per voxel: %d\n", niiReader.getBitpix(outputBytesVector));

    printf("data type: %d\n", niiReader.getDataType(outputBytesVector));
    
    float voxOffset = niiReader.getVoxelOffset(outputBytesVector);
    printf("voxel offset: %f\n", voxOffset);

    // while(true) {
    //     cout << "\nEnter x, y, z coordinates: ";
    //     int x, y, z;
    //     cin >> x >> y >> z;

    //     short voxelValue = niiReader.getVoxelValue(outputBytesVector, dim, voxOffset, x, y, z);
    //     printf("Voxel value at (%d, %d, %d): %d\n", x, y, z, voxelValue);
        
    //     cout << "Do you want to continue? (y/n): ";
    //     char choice;
    //     cin >> choice;
    //     if (choice != 'y' && choice != 'Y') { break; }
    // }
    
    // 訪問所有 Voxel 值
    int max = 0;
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    for (int z = 0; z < dim[3]; ++z) {
        for (int y = 0; y < dim[2]; ++y) {
            for (int x = 0; x < dim[1]; ++x) {
                short voxelValue = niiReader.getVoxelValue(outputBytesVector, dim, voxOffset, x, y, z);
                if (voxelValue > max) {
                    max = voxelValue;
                }
            }
        }
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_used = 
    std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    printf("\nFinding max value costs: %f s\n", time_used.count());
    printf("max voxel value: %d\n", max);
    return 0;
}