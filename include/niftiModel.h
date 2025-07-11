#ifndef NIFTIMODEL_H
#define NIFTIMODEL_H

#include <string>
#include <limits>
#include "model.h"
#include "shaderReader.h"
#include "niftiReader.h"

using namespace std;

class niftiModel : public model {
    public:
        niftiModel(const string niftiFilePath, glm::mat4 &mvMatrixClipPlane);
        ~niftiModel() {}

        niftiReader niftiReaderInsdance; // Nifti 檔案讀取器實例
        glm::mat4 mvMatrixVisible; // 裁切平面的姿態

        // 繪製函式
        void draw() override;
        void translate(glm::vec3 axis) override;
        void rotate(float angle, glm::vec3 axis) override;

        // shader file 讀取器
        shaderReader shaderReaderInstance;

        // 3D 紋理數據
        GLuint volume;

        // 緩衝區和著色器程序的ID
        GLuint vao;
        GLuint vbo;
        GLuint program;

        // 頂點和片段著色器的ID
        GLuint vs;
	    GLuint fs;

        // 該模型的所有頂點位置
        GLfloat* vertexPositions;

        // 3D 紋理位置
        GLint volumeLocation;

        // 裁切平面位置
        glm::mat4* mvMatrixClipPlane; // 裁切平面矩陣姿態
        GLint clipPlaneLocation; 
};

#endif