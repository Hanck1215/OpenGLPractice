#include "cubeModel.h"

cubeModel::cubeModel() {
    // 設定頂點數據
    vertexPositions = new GLfloat[36 * 3] {
        -0.25f,  0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        0.25f, -0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        0.25f, -0.25f,  0.25f,
        0.25f,  0.25f, -0.25f,

        0.25f, -0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,
        0.25f,  0.25f, -0.25f,

        0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        0.25f, -0.25f,  0.25f,
        0.25f, -0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f,  0.25f,

        -0.25f,  0.25f, -0.25f,
        0.25f,  0.25f, -0.25f,
        0.25f,  0.25f,  0.25f,

        0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f, -0.25f
    };
    
    // 初始化著色器讀取器
    shaderReaderInstance = shaderReader();
    program = glCreateProgram(); // 創建著色器程序
    vs = glCreateShader(GL_VERTEX_SHADER); // 創建頂點著色器
    fs = glCreateShader(GL_FRAGMENT_SHADER); // 創建片段著色器

    // 讀取著色器檔案並將其轉換為二進位數據
    char** vsBytes;
    char** fsBytes;
    shaderReaderInstance.readShaderAsBytes("./glsl/testShader.vs.glsl", vsBytes);
    shaderReaderInstance.readShaderAsBytes("./glsl/testShader.fs.glsl", fsBytes);

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
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), vertexPositions, GL_STATIC_DRAW);
}

// 平移模型視圖矩陣
void cubeModel::translate(glm::vec3 axis) { 
    mvMatrix = glm::translate(mvMatrix, axis); 
}

// 旋轉模型視圖矩陣
void cubeModel::rotate(float angle, glm::vec3 axis) {
    mvMatrix = glm::rotate(mvMatrix, glm::radians(angle), axis);
}

void cubeModel::draw() {
    // 使用著色器程序
    glUseProgram(program);

    /*
    啟用深度寫入 : 
    如果設為 GL_FALSE，
    繪圖時就不會更新深度值，通常用於透明物件或特殊效果。
    設為 GL_TRUE 是一般 3D 物件繪製的預設，確保物件深度能正確判斷遮擋關係。*/ 
    glDepthMask(GL_TRUE); 

    /*
    啟用深度測試 : 
    深度測試會比較每個像素的深度值，
    只有比目前緩衝區更「近」的像素才會被繪製，
    這樣才能正確顯示前後遮擋的 3D 效果。*/ 
    glEnable(GL_DEPTH_TEST);

    // 更新姿態矩陣和投影矩陣
    glUniformMatrix4fv(mvLocation, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projMatrix[0][0]);

    // 綁定 VAO 並繪製立方體
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}