#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))
#define rad2deg(x) ((180.0f) / ((x)*(3.1415926f)))

class model {
    public:
        // Constructor and destructor
        model() {}
        virtual ~model() {}

        // Virtual function to draw the model
        virtual void draw() = 0;
        virtual void translate(glm::vec3 axis) = 0;
        virtual void rotate(float angle, glm::vec3 axis) = 0;

        // 模型視圖和投影矩陣的位置
        GLint mvLocation;
        GLint projLocation;
        
        // 模型視圖和投影矩陣的矩陣
        glm::mat4 mvMatrix;
        glm::mat4 projMatrix;
};

#endif