#ifndef SHADERREADER_H
#define SHADERREADER_H

#include <vector>
#include <string>
#include <ios>
#include <fstream>
#include <cstring>
#include <iostream>
#include <GL/glew.h>

using namespace std;

class shaderReader {
    public:
        // 空的建構子與解構子
        shaderReader() {};
        ~shaderReader() {};

        // 根據指定的 shader 檔案路徑，讀取該 shader 檔案為二進位數據
        void readShaderAsBytes(const string &shaderFilePath, char** &shaderBytes);

        // 釋放 shaderBytes 的記憶體
        void freeShaderBytes(char** &shaderBytes);

        // 檢查 shader 編譯是否成功，並輸出錯誤日誌
        void shaderLog(GLuint shader);

        // 輸出 OpenGL 的相關資訊
        inline static void dumpInfo() {
            printf("Vendor: %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version: %s\n", glGetString(GL_VERSION));
            printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        };
};

#endif