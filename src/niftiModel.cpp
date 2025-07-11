#include "niftiModel.h"

niftiModel::niftiModel(const string niftiFilePath, glm::mat4 &mvMatrixClipPlane) {
    // 頂點數據 (頂點位置、材質座標)
    float data[] = {
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,

        0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,

        0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,

        0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,

        0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,

        0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

        0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f
	};

    // 初始化著色器讀取器
    shaderReaderInstance = shaderReader();
    program = glCreateProgram(); // 創建著色器程序
    vs = glCreateShader(GL_VERTEX_SHADER); // 創建頂點著色器
    fs = glCreateShader(GL_FRAGMENT_SHADER); // 創建片段著色器

    // 讀取著色器檔案並將其轉換為二進位數據
    char** vsBytes;
    char** fsBytes;
    shaderReaderInstance.readShaderAsBytes("./glsl/niftiShader.vs.glsl", vsBytes);
    shaderReaderInstance.readShaderAsBytes("./glsl/niftiShader.fs.glsl", fsBytes);

    // 設定著色器源碼
    glShaderSource(vs, 1, vsBytes, NULL);
	glShaderSource(fs, 1, fsBytes, NULL);

    // 釋放著色器檔案的記憶體
    shaderReaderInstance.freeShaderBytes(vsBytes);
    shaderReaderInstance.freeShaderBytes(fsBytes);

    // 編譯著色器
    glCompileShader(vs);
    shaderReaderInstance.shaderLog(vs); // 檢查頂點著色器編譯是否成功

    glCompileShader(fs);
    shaderReaderInstance.shaderLog(fs); // 檢查片段著色器編譯是否成功

    // 將著色器附加到程序
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // 獲取模型視圖和投影矩陣的位置
    mvLocation = glGetUniformLocation(program, "mvMatrix");
    projLocation = glGetUniformLocation(program, "projMatrix");
    volumeLocation = glGetUniformLocation(program, "volume"); // 獲取 3D 紋理位置

    // 初始化模型視圖和投影矩陣
    mvMatrix = glm::mat4(1.0f);
    projMatrix = glm::mat4(1.0f);

    // 創建頂點陣列對象和頂點緩衝區對象，然後綁定
    glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);

    // 設定頂點屬性指針
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

    // 將頂點數據上傳到緩衝區
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW); 

    // 讀取 Nifti 檔案為 bytes 向量
    vector<char> bytesVector;
    niftiReaderInsdance.readNiftiAsBytes(niftiFilePath, bytesVector);

    // 取得 Data array dimensions
    vector<short> dim;
    niftiReaderInsdance.getDimension(bytesVector, dim);

    // 取得 Voxel offset
    float voxelOffset = niftiReaderInsdance.getVoxelOffset(bytesVector);

    // 尋找最大體素值
    short maxValue = std::numeric_limits<short>::lowest(); // 初始化最大值為最小的 short 值
    for (int z = 0; z < dim[3]; ++z) {
        for (int y = 0; y < dim[2]; ++y) {
            for (int x = 0; x < dim[1]; ++x) {
                // 取得對應的 Voxel 值
                short voxelValue = niftiReaderInsdance.getVoxelValue(bytesVector, dim, voxelOffset, x, y, z);

                // 更新最大值
                if (voxelValue > maxValue) { maxValue = voxelValue; }
            }
        }
    }

    // 計算縮放係數
    float scaleFactor = 255.0f / static_cast<float>(maxValue);

    // 把每個體素讀取出來並儲存
    unsigned char* volumeData = new unsigned char[256 * 256 * 256]; // 假設體積大小為 256x256x256
    
    for (int z = 0; z < 256; ++z) {
        for (int y = 0; y < 256; ++y) {
            for (int x = 0; x < 256; ++x) {
                // 取得對應的 Voxel 值，同時壓縮到 0-255 範圍內
                if(x > 31 && x < 224) {
                    short voxelValue = static_cast<short>(
                        scaleFactor * static_cast<float>(
                            niftiReaderInsdance.getVoxelValue(bytesVector, dim, voxelOffset, x-32, y, z)
                        )
                    );
                    
                    // 儲存到 volumeData 中
                    size_t index = x + y * 256 + z * 256 * 256;
                    volumeData[index] = static_cast<unsigned char>(voxelValue);
                }
            }
        }
    }

    // 創建 3D 紋理數據
    glGenTextures(1, &volume);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volume);
    glUniform1i(volumeLocation, 0);
    
    // 設置紋理參數
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 把數據上傳到 GPU
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 256, 256, 256, 0, GL_RED, GL_UNSIGNED_BYTE, volumeData);
    delete[] volumeData; // 釋放體積數據

    // 設置裁切平面矩陣
    this->mvMatrixClipPlane = &mvMatrixClipPlane;
    clipPlaneLocation = glGetUniformLocation(program, "clipPlaneMatrix");
}

// 平移模型視圖矩陣
void niftiModel::translate(glm::vec3 axis) { 
    mvMatrix = glm::translate(mvMatrix, axis); 
}

// 旋轉模型視圖矩陣
void niftiModel::rotate(float angle, glm::vec3 axis) {
    mvMatrix = glm::rotate(mvMatrix, glm::radians(angle), axis);
}

void niftiModel::draw() {
    // 使用著色器程序
    glUseProgram(program);

    glDepthMask(GL_FALSE); // 禁用深度寫入，確保切片不會被其他物體遮擋

    // 更新模型視圖和投影矩陣
    glUniformMatrix4fv(mvLocation, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projMatrix[0][0]);
    glUniformMatrix4fv(clipPlaneLocation, 1, GL_FALSE, &(*mvMatrixClipPlane)[0][0]);

    // 綁定 VAO 並繪製模型
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}