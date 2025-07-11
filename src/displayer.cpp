#include "displayer.h"

vector<model*> Displayer::models; // 靜態成員變量，用於存儲模型列表
int Displayer::btnDownX = 0; // 初始化滑鼠按下位置 X 座標
int Displayer::btnDownY = 0; // 初始化滑鼠按下位置 Y 座標
bool Displayer::dragging = false; // 初始化拖動狀態
bool Displayer::rotationMode = true; // 初始化為旋轉模式
bool Displayer::transitionMode = false; // 初始化為非平移模式
bool Displayer::freezeMode = false; // 初始化為非鎖定模式
size_t Displayer::modelIndex = -1; // 初始化模型索引

Displayer::Displayer(int argc, char** argv) {
    // 初始化 GLUT
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);

    // 設定視窗位置和大小
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("myOpenGL");

    // 初始化 GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    dumpInfo(); // 輸出 OpenGL 的相關資訊

    // OpenGL 狀態設定
    glClearDepth(1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 設定菜單
    int mainMenu = glutCreateMenu(menu);
	glutSetMenu(mainMenu);
	glutAddMenuEntry("ROTATION_MODE", ROTATION_MODE);
	glutAddMenuEntry("TRANSITION_MODE", TRANSITION_MODE);
    glutAddMenuEntry("FREEZE_MODE", FREEZE_MODE);
    glutAddMenuEntry("NEXT", NEXT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

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
    #define stride(x) min(pow(0.1f * x, 2.0f), 0.5f) // 根據深度計算移動步長
    else if(button == 3 || button == 4) {
        if(modelIndex != -1) {
            if(freezeMode) {
                // 計算模型中心在視圖中的深度
                glm::vec4 depth = models[modelIndex]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                for(size_t i = 0; i < models.size(); ++i) {
                    if (models[i] != nullptr) {
                        glm::vec4 o = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                        glm::vec4 p = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                        glm::vec3 zAxis = glm::normalize(p-o); // 計算 Z 軸方向 (模型坐標系)

                        // 根據滾輪方向決定移動方向
                        zAxis = (button == 3 ? stride(abs(depth.z)) * zAxis : -stride(abs(depth.z)) * zAxis);
                        models[i]->translate(zAxis); // 向前移動模型
                    } else {
                        std::cerr << "Model at index " << i << " is null!" << std::endl;
                    }

                }
            }
            else if (models[modelIndex] != nullptr) {
                glm::vec4 o = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                glm::vec4 p = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                glm::vec3 zAxis = glm::normalize(p-o); // 計算 Z 軸方向 (模型坐標系)

                // 計算模型中心在視圖中的深度
                glm::vec4 depth = models[modelIndex]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                // 根據滾輪方向決定移動方向
                // 如果深度小於 20.0f，則移動模型
                zAxis = (button == 3 ? stride(abs(depth.z)) * zAxis : -stride(abs(depth.z)) * zAxis);
                models[modelIndex]->translate(zAxis); // 向前移動模型
            } else {
                std::cerr << "Model at index " << modelIndex << " is null!" << std::endl;
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

        if(!models.empty()) {
            if (models[modelIndex] != nullptr) {
                glm::mat4 mvMatrixOld = models[modelIndex]->mvMatrix; // 保存舊的模型視圖矩陣
                glm::vec4 o = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                glm::vec4 p = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                glm::vec3 xAxis = glm::normalize(p-o); // 計算 X 軸方向 (模型坐標系)

                o = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                p = glm::inverse(models[modelIndex]->mvMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                glm::vec3 yAxis = glm::normalize(p-o); // 計算 Y 軸方向 (模型坐標系)

                // 計算模型中心在視圖中的深度
                glm::vec4 depth = models[modelIndex]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                // 繞 X 軸和 Y 軸旋轉或平移模型 (模型坐標系)
                if(rotationMode) {
                    models[modelIndex]->rotate(dy * 0.1f, xAxis);
                    models[modelIndex]->rotate(dx * 0.1f, yAxis);
                } else if(transitionMode) {
                    models[modelIndex]->translate(0.05f * stride(abs(depth.z)) * dx * xAxis); // 平移模型
                    models[modelIndex]->translate(-0.05f * stride(abs(depth.z)) * dy * yAxis); // 平移模型
                } else {
                    std::cerr << "Unknown mode!" << std::endl;
                }

                if(freezeMode) {
                    for(size_t i = 0; i < models.size(); ++i) {
                        if(i != modelIndex && models[i] != nullptr) {
                            //計算該模型原點在世界座標中的位置和主要模型坐標系的位置
                            glm::vec4 ow = models[i]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                            glm::vec4 oMain = glm::inverse(mvMatrixOld) * ow;

                            glm::vec4 a = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                            glm::vec4 b = glm::inverse(models[i]->mvMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                            xAxis = glm::normalize(b-a);

                            a = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                            b = glm::inverse(models[i]->mvMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                            yAxis = glm::normalize(b-a);

                            // 繞 X 軸和 Y 軸旋轉或平移模型 (模型坐標系)
                            if(rotationMode) {
                                models[i]->rotate(dy * 0.1f, xAxis);
                                models[i]->rotate(dx * 0.1f, yAxis);

                                // //計算該物件原點需要在世界座標中的位置
                                glm::vec4 owNew = models[modelIndex]->mvMatrix * oMain;

                                // //計算到達新位置需要的向量
                                glm::vec4 owNewVector = glm::inverse(models[i]->mvMatrix) * owNew;

                                // //移動到新的位置
                                models[i]->translate(glm::vec3(owNewVector));
                            } 
                            else if(transitionMode) {
                                models[i]->translate(0.05f * stride(abs(depth.z)) * dx * xAxis); // 平移模型
                                models[i]->translate(-0.05f * stride(abs(depth.z)) * dy * yAxis); // 平移模型
                            } else {
                                std::cerr << "Unknown mode!" << std::endl;
                            }
                        }
                    }
                }
            }
            else {
                std::cerr << "Model at index " << modelIndex << " is null!" << std::endl;
            }
        }

        btnDownX = x; // 更新按下位置
        btnDownY = y;
    }
}

// 處理菜單選項的函式
void Displayer::menu(int id) {
    if (id == ROTATION_MODE) {
        rotationMode = true; // 切換到旋轉模式
        transitionMode = false; // 確保平移模式被禁用
    } 
    else if (id == TRANSITION_MODE) {
        rotationMode = false; // 切換到平移模式
        transitionMode = true; // 確保旋轉模式被禁用
    } 
    else if (id == FREEZE_MODE) {
        freezeMode = !freezeMode;
    } 
    else if(id == NEXT) {
        // 切換到下一個模型
        if (models.size() > 0) {
            modelIndex = (modelIndex + 1) % models.size(); // 循環切換模型索引
            std::cout << "Switched to model index: " << modelIndex << std::endl;
        } else {
            std::cerr << "No models available to switch!" << std::endl;
        }
    }
    else {
        std::cerr << "Unknown menu option selected: " << id << std::endl;
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