#include <chrono> // 提供高精度時間測量功能
#include "sliceModel.h"

sliceModel::sliceModel() {
    // 設定頂點數據
    vertexPositions = new GLfloat[18] {
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,

        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    
    // 初始化著色器讀取器
    shaderReaderInstance = shaderReader();
    program = glCreateProgram(); // 創建著色器程序
    vs = glCreateShader(GL_VERTEX_SHADER); // 創建頂點著色器
    fs = glCreateShader(GL_FRAGMENT_SHADER); // 創建片段著色器

    // 讀取著色器檔案並將其轉換為二進位數據
    char** vsBytes;
    char** fsBytes;
    shaderReaderInstance.readShaderAsBytes("./glsl/sliceShader.vs.glsl", vsBytes);
    shaderReaderInstance.readShaderAsBytes("./glsl/sliceShader.fs.glsl", fsBytes);

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

    // 獲取是否被選取變數的位置
    selectedLocation = glGetUniformLocation(program, "selected");

    // 初始化模型視圖和投影矩陣
    mvMatrix = glm::mat4(1.0f);
    projMatrix = glm::mat4(1.0f);

    // 創建頂點陣列對象和頂點緩衝區對象
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // 綁定 VAO 和 VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 設定頂點屬性指針
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertexPositions, GL_STATIC_DRAW);
}

// 平移模型視圖矩陣
void sliceModel::translate(glm::vec3 axis) { 
    glm::vec3 newAxis;
    if(axis.z > 0) {
        newAxis = glm::vec3(0.0f, 0.0f, glm::length(axis));
    }else {
        newAxis = glm::vec3(0.0f, 0.0f, -glm::length(axis));
    }
    mvMatrix = glm::translate(mvMatrix, 0.1f * newAxis);
}

// 旋轉模型視圖矩陣
void sliceModel::rotate(float angle, glm::vec3 axis) {
    mvMatrix = glm::rotate(mvMatrix, glm::radians(angle), axis);
    correction(); // 校正姿態，使得面朝向面向中心
}

// 校正姿態，使得面朝向面向中心
void sliceModel::correction() {
    // 如果沒有設置，就不做任何事情
    if(mvMatrixCentral == nullptr) { return; }

    // 計算面向中心在世界坐標系下的位置
    glm::vec4 centralWorld = (*mvMatrixCentral) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // 計算面向中心在本裁切平面坐標系下的位置
    glm::vec4 centralSlice = glm::inverse(mvMatrix) * centralWorld;

    // 計算面向中心的向量投影到本裁切平面朝向向量的向量
    glm::vec3 projCentral = glm::dot(glm::vec3(centralSlice), glm::vec3(0,0,-1)) * glm::vec3(0,0,-1);

    // 計算平移方向，使得本裁切平面朝向面向中心 (指向被減數)
    glm::vec3 correctionVector = glm::vec3(centralSlice) - projCentral;

    // 校正姿態
    mvMatrix = glm::translate(mvMatrix, correctionVector);
}

void sliceModel::draw() {
    // 使用著色器程序
    glUseProgram(program);

    glDepthMask(GL_FALSE); // 禁用深度寫入，確保切片不會被其他物體遮擋

    // 更新模型視圖和投影矩陣
    // 更新狀態到 GPU 上
    glUniform1i(selectedLocation, selected ? 1 : 0);
    glUniformMatrix4fv(mvLocation, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projMatrix[0][0]);

    // 綁定 VAO 並繪製立方體
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}