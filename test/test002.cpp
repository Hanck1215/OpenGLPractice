#include "displayer.h"
#include "niftiModel.h"
#include "sliceModel.h"

int main(int argc, char **argv) {
    // 創建 Displayer 實例
    Displayer displayer(argc, argv); 

    model* sliceModelInstace = new sliceModel(); // 創建立方體模型實例
    model* niftiModelInstance = new niftiModel("../../testData/T1.nii", sliceModelInstace->mvMatrix); // 創建立方體模型實例
    niftiModelInstance->mvMatrix = glm::translate(niftiModelInstance->mvMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
    displayer.appendModel(niftiModelInstance); // 將立方體模型加入模型
    
    ((sliceModel*)sliceModelInstace)->setMatrixCentral(niftiModelInstance->mvMatrix);
    sliceModelInstace->mvMatrix = glm::translate(sliceModelInstace->mvMatrix, glm::vec3(0.0f, 0.0f, -3.0f)); // 平移模型視圖矩陣
    displayer.appendModel(sliceModelInstace); // 將立方體模型加入模型

    displayer.startDisplayLoop(); // 啟動顯示循環
    return 0;
}