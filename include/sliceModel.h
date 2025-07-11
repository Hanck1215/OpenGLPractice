#ifndef SLICEMODEL_H
#define SLICEMODEL_H

#include "shaderReader.h"
#include "model.h"

class sliceModel : public model {
    public:
        sliceModel();
        ~sliceModel() {}

        // 繪製函式
        void draw() override;
        void translate(glm::vec3 axis) override;
        void rotate(float angle, glm::vec3 axis) override;

        // shader file 讀取器
        shaderReader shaderReaderInstance;

        // 緩衝區和著色器程序的ID
        GLuint vao;
        GLuint vbo;
        GLuint program;

        // 頂點和片段著色器的ID
        GLuint vs;
	    GLuint fs;

        // 該模型的所有頂點位置
        GLfloat* vertexPositions;
};

#endif
