#ifndef DISPLAYER_H
#define DISPLAYER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cstdio>
#include <iostream>
#include "cubeModel.h"

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))
#define rad2deg(x) ((180.0f) / ((x)*(3.1415926f)))

cubeModel* cube = nullptr; // 立方體模型的指標
cubeModel* cube2 = nullptr; // 立方體模型的指標

void display();
void reshape(int width, int height);
void timer(int value);

void initializeGL(int argc, char** argv);

void dumpInfo() {
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
};

#endif