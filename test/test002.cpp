#include "displayer.h"
#include "niftiModel.h"

int main(int argc, char **argv) {
    // 創建 Displayer 實例
    Displayer displayer(argc, argv); 

    model* niftiModelInstance = new niftiModel("../../testData/T1.nii"); // 創建立方體模型實例
    niftiModelInstance->translate(glm::vec3(0.0f, 0.0f, -3.0f)); // 平移模型視圖矩陣
    Displayer::models.push_back(niftiModelInstance); // 將立方體模型加入模型

    displayer.startDisplayLoop(); // 啟動顯示循環
    return 0;
}