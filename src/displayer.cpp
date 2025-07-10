#include "displayer.h"

vector<model*> Displayer::models; // 靜態成員變量，用於存儲模型列表
int Displayer::btnDownX = 0; // 初始化滑鼠按下位置 X 座標
int Displayer::btnDownY = 0; // 初始化滑鼠按下位置 Y 座標
bool Displayer::dragging = false; // 初始化拖動狀態

Displayer::Displayer(int argc, char** argv) {
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

    glutDisplayFunc(display); // 設定顯示函式
    glutReshapeFunc(reshape); // 設定視窗變動時的回調函式
    glutMouseFunc(mouse); // 設定滑鼠事件的回調函式
    glutMotionFunc(mouseMotion); // 設定滑鼠拖動事件的回調
    glutTimerFunc(1000 / 60, timer, 1000 / 60); // 設定畫面更新的時間

    models.reserve(20); // 預留空間以提高性能
}

// 處理滑鼠事件的函式
void Displayer::mouse(int button, int state, int x, int y) {
    // 記錄按下時的鼠標座標
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        btnDownX = x; btnDownY = y;
        dragging = true; // 開始拖動
    }
    // 當滑鼠按鍵釋放時，停止拖動
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        dragging = false; // 停止拖動
    }
    // 滾輪向上放大、向下縮小
    #define stride(x) pow(0.1f * x, 2.0f) // 根據深度計算移動步長
    else if(button == 3 || button == 4) {
        for(size_t i = 0; i < models.size(); ++i) {
            if (models[i] != nullptr) {
                glm::vec4 o = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                glm::vec4 p = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                glm::vec3 zAxis = glm::normalize(p-o); // 計算 Z 軸方向 (模型坐標系)

                // 計算模型中心在視圖中的深度
                glm::vec4 depth = models[i]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                // 根據滾輪方向決定移動方向
                zAxis = (button == 3 ? stride(abs(depth.z)) * zAxis : -stride(abs(depth.z)) * zAxis);
                models[i]->translate(zAxis); // 向前移動模型
            } else {
                std::cerr << "Model at index " << i << " is null!" << std::endl;
            }
        }
    }
}

// 處理滑鼠拖動事件的函式
void Displayer::mouseMotion(int x, int y) {
    // 如果正在拖動，計算新的視圖矩陣
    if (dragging) {
        int dx = x - btnDownX; // 計算 X 軸的變化量
        int dy = y - btnDownY; // 計算 Y 軸的變化量

        for(size_t i = 0; i < models.size(); ++i) {
            if (models[i] != nullptr) {
                glm::vec4 o = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                glm::vec4 p = glm::inverse(models[i]->mvMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                glm::vec3 xAxis = glm::normalize(p-o); // 計算 X 軸方向 (模型坐標系)

                o = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                p = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                glm::vec3 yAxis = glm::normalize(p-o); // 計算 Y 軸方向 (模型坐標系)

                // 繞 X 軸和 Y 軸旋轉模型 (模型坐標系)
                models[i]->rotate(dy * 0.1f, xAxis);
                models[i]->rotate(dx * 0.1f, yAxis);
            }
            else {
                std::cerr << "Model at index " << i << " is null!" << std::endl;
            }
        }

        btnDownX = x; // 更新按下位置
        btnDownY = y;
    }
}

// 顯示函式，將在每次需要重繪時被呼叫
void Displayer::display() {
    // 清除緩衝區
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 繪製所有模型
    for(size_t i = 0; i < models.size(); ++i) {
        if (models[i] != nullptr) {
            models[i]->draw(); // 繪製模型
        } else {
            std::cerr << "Model at index " << i << " is null!" << std::endl;
        }
    }

    // 交換緩衝區
    glutSwapBuffers();
}

// 設定視窗變動時的回調函式
void Displayer::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    float viewportAspect = (float)width / (float)height;

    for(size_t i = 0; i < models.size(); ++i) {
        if (models[i] != nullptr) {
            models[i]->projMatrix = glm::perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
        } else {
            std::cerr << "Model at index " << i << " is null!" << std::endl;
        }
    }
}

// 設定畫面更新的時間
void Displayer::timer(int value) {
    glutPostRedisplay(); // 重新繪製畫面
    glutTimerFunc(value, timer, value); // 設定下一次的計時器
};

