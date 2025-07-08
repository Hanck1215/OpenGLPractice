#include "niftiReader.h"
#include <cstdio>
int main(int argc, char **argv) {
    const string path = argv[1];
    vector<char> outputBytesVector;
    niftiReader niiReader;
    niiReader.readNiftiAsBytes(path, outputBytesVector);
    int* iptr = reinterpret_cast<int*>(&outputBytesVector[0]);
    printf("%d\n", *iptr);
}