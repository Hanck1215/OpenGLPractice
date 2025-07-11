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
    glClearDepth(1.0f); // 確保每次繪圖前，深度緩衝區都被初始化為「最遠的距離」（1.0）。避免前一幀的深度資料會影響新一幀的繪製結果
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 設定清除顏色緩衝區時的顏色為白色（RGBA: 1,1,1,1）。每次 glClear(GL_COLOR_BUFFER_BIT) 時，畫面背景會變成白色。
    glEnable(GL_DEPTH_TEST); // 啟用深度測試。繪製像素時會根據深度值判斷遮擋關係，確保近的物件會蓋住遠的物件。
    glDepthFunc(GL_LEQUAL); // 設定深度測試的比較函式為「小於或等於」。這意味著當新像素的深度值小於或等於緩衝區中的深度值時，該像素會被繪製。
    glEnable(GL_BLEND); // 啟用混合功能。這允許我們在繪製透明物件時，能夠正確地混合顏色。
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 設定混合方式。這是標準的 alpha blending，讓物件可以有透明度效果：螢幕顏色 = 物件顏色 * alpha + 背景顏色 * (1 - alpha)

    // 設定選單
    int mainMenu = glutCreateMenu(menu);
	glutSetMenu(mainMenu);
	glutAddMenuEntry("ROTATION_MODE", ROTATION_MODE); // 滑鼠旋轉模式
	glutAddMenuEntry("TRANSITION_MODE", TRANSITION_MODE); // 滑鼠平移模式
    glutAddMenuEntry("FREEZE_MODE", FREEZE_MODE); // 以當前選擇之模型為中心，同時操作所有物件
    glutAddMenuEntry("NEXT", NEXT); // 切換到下一個模型
	glutAttachMenu(GLUT_RIGHT_BUTTON); // 右鍵點擊時顯示選單

    glutDisplayFunc(display); // 設定顯示函式
    glutReshapeFunc(reshape); // 設定視窗變動時的回調函式
    glutMouseFunc(mouse); // 設定滑鼠事件的回調函式
    glutMotionFunc(mouseMotion); // 設定滑鼠拖動事件的回調
    glutTimerFunc(1000 / 60, timer, 1000 / 60); // 設定畫面更新的時間

    models.reserve(20); // 預留模型列表的空間以提高性能
}

// 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
void getXYZAxisWorld2Model(size_t index, glm::vec3 &xAxis, glm::vec3 &yAxis, glm::vec3 &zAxis) {
    // 將世界座標系下的 x 軸向量轉換到該模型的座標系下
    glm::vec4 o = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 p = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    xAxis = glm::normalize(p-o); // 計算 X 軸方向 (模型坐標系)

    // 將世界座標系下的 y 軸向量轉換到該模型的座標系下
    o = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    p = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    yAxis = glm::normalize(p-o); // 計算 Y 軸方向 (模型坐標系)

    // 將世界座標系下的 Z 軸向量轉換到該模型的座標系下
    o = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    p = glm::inverse(Displayer::models[index]->mvMatrix) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    zAxis = glm::normalize(p-o); // 計算 Z 軸方向 (模型坐標系)
}

// 縮放模型列表中指定索引的模型
#define stride(x) min(pow(0.1f * x, 2.0f), 0.5f) // 根據深度計算移動步長
void zoomModel(int button, size_t index, glm::vec4 &oWorld) {
    // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
    glm::vec3 xAxis, yAxis, zAxis;
    getXYZAxisWorld2Model(index, xAxis, yAxis, zAxis);

    // 根據滾輪方向決定靠近或遠離，同時根據指定模型之姿態矩陣計算距離鏡頭之深度決定移動速度
    // 越近則移動越慢；越遠則移動越快
    zAxis = (button == 3 ? stride(abs(oWorld.z)) * zAxis : -stride(abs(oWorld.z)) * zAxis);
    Displayer::models[index]->translate(zAxis);
}

// 同時縮放模型列表中所有模型
void zoomAllModel(int button) {
    // 先計算當前模型(索引為modelIndex)中心點在世界坐標系下的座標
    glm::vec4 oWorld = Displayer::models[Displayer::modelIndex]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for(size_t i = 0; i < Displayer::models.size(); ++i) { // 遍歷模型列表
        // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
        glm::vec3 xAxis, yAxis, zAxis;
        getXYZAxisWorld2Model(i, xAxis, yAxis, zAxis);

        // 根據滾輪方向決定靠近或遠離，同時根據指定模型之姿態矩陣計算距離鏡頭之深度決定移動速度
        // 越近則移動越慢；越遠則移動越快
        zAxis = (button == 3 ? stride(abs(oWorld.z)) * zAxis : -stride(abs(oWorld.z)) * zAxis);
        Displayer::models[i]->mvMatrix = glm::translate(Displayer::models[i]->mvMatrix, zAxis); 
    }
}

// 滑鼠滾動時的處理函數
void mouseScroll(int button) {
    if(!Displayer::models.empty()) { // 如果模型列表中有模型
        if(Displayer::freezeMode) { // 如果是鎖定模式
            zoomAllModel(button); // 縮放所有模型
        }else {
            // 計算當前模型中心在視圖中的深度
            glm::vec4 oWorld = Displayer::models[Displayer::modelIndex]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            zoomModel(button, Displayer::modelIndex, oWorld); // 縮放指定模型
        }
    }
}

// 處理滑鼠事件的函式
void Displayer::mouse(int button, int state, int x, int y) {
    // 處理滑鼠按下事件
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        btnDownX = x; btnDownY = y; // 記錄按下時的鼠標座標
        dragging = true; // 狀態為開始拖動
    }
    // 當滑鼠按鍵釋放時
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        dragging = false; // 狀態為停止拖動
    }
    // 當滾輪滾動時
    else if(button == 3 || button == 4) { // 3 表示滾輪向前滾動，4 表示滾輪向後滾動
        mouseScroll(button); // 縮放模型
    }
}

// 在世界坐標系的觀點下旋轉模型
void rotateModelWorld(int index, int dx, int dy) {
    // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
    glm::vec3 xAxis, yAxis, zAxis;
    getXYZAxisWorld2Model(index, xAxis, yAxis, zAxis);

    // 先計算模型(索引為index)中心點在世界坐標系下的座標
    glm::vec4 oWorld = Displayer::models[index]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // 繞世界坐標系下的 x、y 軸旋轉物件
    Displayer::models[index]->rotate(dy * 0.1f, xAxis);
    Displayer::models[index]->rotate(dx * 0.1f, yAxis);
}

// 在世界坐標系的觀點下平移模型
void translateModelWorld(int index, int dx, int dy) {
    // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
    glm::vec3 xAxis, yAxis, zAxis;
    getXYZAxisWorld2Model(index, xAxis, yAxis, zAxis);

    // 先計算模型(索引為index)中心點在世界坐標系下的座標
    glm::vec4 oWorld = Displayer::models[index]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // 繞世界坐標系下的 x、y 軸平移物件
    Displayer::models[index]->translate(0.05f * stride(abs(oWorld.z)) * dx * xAxis);
    Displayer::models[index]->translate(-0.05f * stride(abs(oWorld.z)) * dy * yAxis);
}

// 鎖定模式下的旋轉方法
// 以指定模型為中心點，旋轉所有物件
void rotateModelWorldFreeze(int centralModelIndex, int dx, int dy) {
    // 保存舊的中央模型姿態
    glm::mat4 mvMatrixOld = Displayer::models[centralModelIndex]->mvMatrix; 

    rotateModelWorld(centralModelIndex, dx, dy); // 旋轉中央模型

    // 旋轉剩下的模型
    for(size_t i = 0; i < Displayer::models.size(); ++i) {
        if(i == centralModelIndex) { continue; } // 跳過中央模型
        // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
        glm::vec3 xAxis, yAxis, zAxis;
        getXYZAxisWorld2Model(i, xAxis, yAxis, zAxis);

        // 計算該模型原點在世界座標中的位置，以及主要模型(索引為modelIndex)坐標系的位置
        glm::vec4 oWorld = Displayer::models[i]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 oMain = glm::inverse(mvMatrixOld) * oWorld;
        
        // 旋轉模型
        Displayer::models[i]->mvMatrix = glm::rotate(Displayer::models[i]->mvMatrix, glm::radians(dy * 0.1f), xAxis); 
        Displayer::models[i]->mvMatrix = glm::rotate(Displayer::models[i]->mvMatrix, glm::radians(dx * 0.1f), yAxis);

        // 計算該物件原點需要在世界座標中的位置
        glm::vec4 oWorldNew = Displayer::models[centralModelIndex]->mvMatrix * oMain;

        // 計算到達新位置需要的向量
        glm::vec4 newVector = glm::inverse(Displayer::models[i]->mvMatrix) * oWorldNew;

        // 移動到新的位置
        Displayer::models[i]->mvMatrix = glm::translate(Displayer::models[i]->mvMatrix, glm::vec3(newVector)); 
    }
}

// 鎖定模式下的平移方法
void translateModelWorldFreeze(int centralModelIndex, int dx, int dy) {
    // 平移所有物件
    for(size_t i = 0; i < Displayer::models.size(); ++i) {
        // 計算世界座標系下的 XYZ 軸向量轉換到指定模型的坐標系下
        glm::vec3 xAxis, yAxis, zAxis;
        getXYZAxisWorld2Model(i, xAxis, yAxis, zAxis);
        
        // 先計算模型中心點在世界坐標系下的座標
        glm::vec4 oWorld = Displayer::models[i]->mvMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // 平移模型
        Displayer::models[i]->mvMatrix = glm::translate(Displayer::models[i]->mvMatrix, 0.05f * stride(abs(oWorld.z)) * dx * xAxis); 
        Displayer::models[i]->mvMatrix = glm::translate(Displayer::models[i]->mvMatrix, -0.05f * stride(abs(oWorld.z)) * dy * yAxis);
    }
}

// 根據是否為鎖定模式選擇旋轉方法
void fRotateModelWorld(bool freezed, size_t index, int dx, int dy) {
    if(!freezed) {
        rotateModelWorld(index, dx, dy);
    }else {
        rotateModelWorldFreeze(index, dx, dy);
    }
}

// 根據是否為鎖定模式選擇平移方法
void fTranslateModelWorld(bool freezed, size_t index, int dx, int dy) {
    if(!freezed) {
        translateModelWorld(index, dx, dy);
    }else {
        translateModelWorldFreeze(index, dx, dy);
    }
}

// 處理滑鼠拖動事件的函式
void Displayer::mouseMotion(int x, int y) {
    // 如果正在拖動，計算新的視圖矩陣
    if (dragging) {
        int dx = x - btnDownX; // 計算 X 軸的變化量
        int dy = y - btnDownY; // 計算 Y 軸的變化量
        
        // 繞世界坐標系下的 x、y 軸，旋轉或平移模型
        if(rotationMode) {
            fRotateModelWorld(freezeMode, modelIndex, dx, dy);
        } else if(transitionMode) {
            fTranslateModelWorld(freezeMode, modelIndex, dx, dy);
        }

        // 更新按下位置
        btnDownX = x; 
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
        freezeMode = !freezeMode; // 切換鎖定模式
    } 
    else if(id == NEXT) {
        modelIndex = (models.size() > 0) ? ((modelIndex + 1) % models.size()) : -1; // 循環切換模型索引
    }
}

// 顯示函式，將在每次需要重繪時被呼叫
void Displayer::display() {
    // 清除緩衝區
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 繪製所有模型
    for(size_t i = 0; i < models.size(); ++i) {
        models[i]->draw(); // 繪製模型
    }

    // 交換緩衝區
    glutSwapBuffers();
}

// 設定視窗變動時的回調函式
void Displayer::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    float viewportAspect = (float)width / (float)height;

    for(size_t i = 0; i < models.size(); ++i) {
        models[i]->projMatrix = glm::perspective(deg2rad(50.0f), viewportAspect, 0.1f, 100.0f);
    }
}

// 設定畫面更新的時間
void Displayer::timer(int value) {
    glutPostRedisplay(); // 重新繪製畫面
    glutTimerFunc(value, timer, value); // 設定下一次的計時器
};