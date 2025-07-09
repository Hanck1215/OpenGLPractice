#include "shaderReader.h"

void shaderReader::readShaderAsBytes(const string &shaderFilePath, char** &shaderBytes) {
    // 以二進位模式讀取檔案
    ifstream shaderFile(shaderFilePath, ios_base::binary);
    
    // 檢查檔案是否成功開啟
    if (!shaderFile.is_open()) {
        throw runtime_error("Failed to open shader file: " + shaderFilePath);
    }

    // 計算檔案有幾 bytes
    shaderFile.seekg(0, ios_base::end);
    size_t length = shaderFile.tellg();
    shaderFile.seekg(0, ios_base::beg);

    // 分配記憶體並讀取檔案內容
    shaderBytes = new char*[1];
    shaderBytes[0] = new char[length + 1];
    shaderFile.read(shaderBytes[0], length);
    shaderBytes[0][length] = '\0'; // 確保字串結尾有 null 字元

    // 關閉檔案
    shaderFile.close();
}

// 釋放 shaderBytes 的記憶體
void shaderReader::freeShaderBytes(char** &shaderBytes) {
    if (shaderBytes != nullptr) {
        delete[] shaderBytes[0]; // 釋放第一個指標指向的記憶體
        delete[] shaderBytes; // 釋放指標陣列本身
        shaderBytes = nullptr; // 避免懸空指標
    }
};

// 檢查 shader 編譯是否成功，並輸出錯誤日誌
void shaderReader::shaderLog(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0) {
            vector<char> log(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log.data());
            throw runtime_error("Shader compilation failed: " + string(log.data()));
        } else {
            throw runtime_error("Shader compilation failed with no error log.");
        }
    }
}