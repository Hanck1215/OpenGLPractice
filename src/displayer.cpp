#include "displayer.h"

// 顯示函式，將在每次需要重繪時被呼叫
void display() {
    // 清除緩衝區
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cube->mvMatrix = glm::mat4(1.0f); // 初始化模型視圖矩陣
    cube->mvMatrix = glm::translate(cube->mvMatrix, glm::vec3(0.0f, 0.0f, -3.0f)); // 平移到Z軸-2的位置
    cube->draw(); // 繪製立方體模型

    cube2->mvMatrix = glm::mat4(1.0f); // 初始化模型視圖矩陣
    cube2->mvMatrix = glm::translate(cube2->mvMatrix, glm::vec3(0.2f, 0.0f, -2.0f)); // 平移到Z軸-2的位置
    cube2->draw(); // 繪製立方體模型

    // 交換緩衝區
    glutSwapBuffers();
}

// 設定視窗變動時的回調函式
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    float viewportAspect = (float)width / (float)height;
    cube->projMatrix = glm::perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
    cube2->projMatrix = glm::perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
}

// 設定畫面更新的時間
void timer(int value) {
    glutPostRedisplay(); // 重新繪製畫面
    glutTimerFunc(value, timer, value); // 設定下一次的計時器
};

void initializeGL(int argc, char** argv) {
    // 初始化 GLUT
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);

    // 設定視窗位置和大小
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(480, 640);
    glutCreateWindow("myOpenGL");

    // 初始化 GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    dumpInfo(); // 輸出 OpenGL 的相關資訊

    // OpenGL 狀態設定
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    cube = new cubeModel(); // 創建立方體模型實例
    cube2 = new cubeModel(); // 創建立方體模型實例

    glutDisplayFunc(display); // 設定顯示函式
    glutReshapeFunc(reshape); // 設定視窗變動時的回調函式
    glutTimerFunc(1000 / 60, timer, 1000 / 60); // 設定畫面更新的時間
};

