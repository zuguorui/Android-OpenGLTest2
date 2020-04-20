//
// Created by 祖国瑞 on 2020-02-15.
//

#ifndef OPENGLTEST_PICPREVIEWER_H
#define OPENGLTEST_PICPREVIEWER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <list>
#include "EGLCore.h"
#include "Render.h"
#include "Texture.h"
#include "IVideoFrameProvider.h"


using namespace std;

class PicPreviewer {
public:
    PicPreviewer();
    ~PicPreviewer();
    bool start();
    bool stop();
    void setWindow(ANativeWindow *window);
    void resetSize(int width, int height);
    void changeColor();
    void renderLoop();

    void setProvider(IVideoFrameProvider *provider);
private:
    Texture *picTexture;
    Render *picRender;
    EGLCore *eglCore;
    EGLSurface previewSurface;

    int width;
    int height;
    enum RenderThreadMessage{
        NONE = 0, SET_WINDOW, RESET_SIZE, EXIT, CHANGE_COLOR
    };

    thread *renderThread = NULL;
    mutex renderMu;
    condition_variable newMsgSignal;

//    RenderThreadMessage message = NONE;

    ANativeWindow *window;

//    static void *threadCallback(void *context);



    bool init();

    void updateTexImage();

    void destroy();

    void drawFrame();

    list<RenderThreadMessage> msgQueue;

    int color = 0;

    IVideoFrameProvider *provider = NULL;

};


#endif //OPENGLTEST_PICPREVIEWER_H
