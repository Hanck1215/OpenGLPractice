#ifndef CUBEMODEL_H
#define CUBEMODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shaderReader.h"

class cubeModel {
    public:
        // Constructor and destructor
        cubeModel();
        ~cubeModel() {};

        // 繪製函式
        void draw();

        // shader file 讀取器
        shaderReader shaderReaderInstance;

        // 模型視圖和投影矩陣的位置
        GLint mvLocation;
        GLint projLocation;
        
        // 模型視圖和投影矩陣的矩陣
        glm::mat4 mvMatrix;
        glm::mat4 projMatrix;

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