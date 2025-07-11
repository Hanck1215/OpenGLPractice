#ifndef DISPLAYER_H
#define DISPLAYER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
#include <iostream>
#include "model.h"
#include "cubeModel.h"

#define ROTATION_MODE	1
#define TRANSITION_MODE	2
#define FREEZE_MODE		3

class Displayer {
    public:
        // 建構函式和解構函式
        Displayer(int argc, char** argv);
        ~Displayer() {}

        // 啟動顯示循環
        void startDisplayLoop() { glutMainLoop(); }

        // callback 函式
        static void display();
        static void reshape(int width, int height);
        static void timer(int value);
        static void mouse(int button, int state, int x, int y);
        static void mouseMotion(int x, int y);
        static void menu(int id);

        // 添加模型到模型列表
        void appendModel(model* m) {
            models.push_back(m);
            if (models.size() == 1) {
                modelIndex = 0; // 如果是第一個模型，設置索引為 0
            }
        }

        // 顯示資訊
        void dumpInfo() {
            printf("Vendor: %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version: %s\n", glGetString(GL_VERSION));
            printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        };

    public:
        static vector<model*> models; // 模型列表
        static int btnDownX, btnDownY; // 滑鼠按下位置
        static bool dragging; // 是否正在拖動
        static bool rotationMode; // 是否為旋轉模式
        static bool transitionMode; // 是否為平移模式
        static bool freezeMode; // 是否為鎖定模式
        static size_t modelIndex; // 當前模型索引
};

#endif