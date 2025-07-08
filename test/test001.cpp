#include "niftiReader.h"
#include <cstdio>
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
    
    return 0;
}