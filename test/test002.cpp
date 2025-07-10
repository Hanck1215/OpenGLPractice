#include "displayer.h"

int main(int argc, char **argv) {
    // 創建 Displayer 實例
    Displayer displayer(argc, argv); 

    model* cube = new cubeModel(); // 創建立方體模型實例
    cube->translate(glm::vec3(0.0f, 0.0f, -3.0f)); // 平移模型視圖矩陣
    Displayer::models.push_back(cube); // 將立方體模型加入模型

    displayer.startDisplayLoop(); // 啟動顯示循環
    return 0;
}