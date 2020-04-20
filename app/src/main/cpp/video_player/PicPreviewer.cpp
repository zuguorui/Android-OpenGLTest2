//
// Created by 祖国瑞 on 2020-02-15.
//

#include "PicPreviewer.h"
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <android/log.h>

using namespace std;
using namespace chrono;

#define MODULE_NAME "PicPreviewer"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

static void threadCallback(void *context) {
    PicPreviewer* picPreviewer = (PicPreviewer *)context;
    picPreviewer->renderLoop();
}

PicPreviewer::PicPreviewer() {
    LOGD("PicPreviewer init");
    eglCore = NULL;
    picTexture = NULL;
    picRender = NULL;
    previewSurface = EGL_NO_SURFACE;

    width = 720;
    height = 1024;
    msgQueue.clear();

}

PicPreviewer::~PicPreviewer() {
    LOGD("PicPreviewer delete");
}

bool PicPreviewer::start() {
    LOGD("start enter");
//    message = NONE;
    renderThread = new thread(threadCallback, this);
    LOGD("start exit");
    return true;

}

bool PicPreviewer::stop() {
    LOGD("stop");
    if(renderThread == NULL)
    {
        return true;
    }
    unique_lock<mutex> locker(renderMu);
//    message = EXIT;
    msgQueue.push_back(RenderThreadMessage::EXIT);
    locker.unlock();
    newMsgSignal.notify_all();
    renderThread->join();
    return true;
}

void PicPreviewer::setProvider(IVideoFrameProvider *provider) {
    this->provider = provider;
}



void PicPreviewer::resetSize(int width, int height) {

    unique_lock<mutex> locker(renderMu);
    LOGD("resetSize enter");
    msgQueue.push_back(RenderThreadMessage::RESET_SIZE);
    this->width = width;
    this->height = height;
    newMsgSignal.notify_all();
    locker.unlock();
    LOGD("resetSize exit");
}

void PicPreviewer::setWindow(ANativeWindow *window) {
    LOGD("set window enter");
    this->window = window;
    unique_lock<mutex> locker(renderMu);
    msgQueue.push_back(RenderThreadMessage::SET_WINDOW);
    newMsgSignal.notify_all();
    locker.unlock();
    LOGD("set window exit");
}

void PicPreviewer::changeColor() {
    LOGD("changeColor");
    unique_lock<mutex> locker(renderMu);
    msgQueue.push_back(RenderThreadMessage::CHANGE_COLOR);
    newMsgSignal.notify_all();
    locker.unlock();
    LOGD("changeColor exit");
}

void PicPreviewer::renderLoop() {
    LOGD("render loop enter");

    bool exitFlag = false;

    RenderThreadMessage message;
    while(!exitFlag)
    {
        LOGD("render loop start");

        unique_lock<mutex> msgLock(renderMu);
        while(msgQueue.size() == 0)
        {
            newMsgSignal.wait(msgLock);
        }
        message = msgQueue.front();
        msgQueue.pop_front();
        switch (message)
        {
            case SET_WINDOW:
                init();
                break;
            case EXIT:
                exitFlag = true;
                break;
            case RESET_SIZE:
                picRender->resetRenderSize(0, 0, width, height);
                break;
            case RenderThreadMessage::CHANGE_COLOR:
            {
                if(eglCore && picRender && picTexture)
                {
                    updateTexImage();
                    eglCore->makeCurrent(previewSurface);
                    drawFrame();
                }
            }
            default:
                break;
        }



        msgLock.unlock();
        LOGD("render loop over");

    }
}

bool PicPreviewer::init() {
    LOGD("init");
    eglCore = new EGLCore();
    eglCore->init();
    previewSurface = eglCore->createWindowSurface(window);
    eglCore->makeCurrent(previewSurface);

    picTexture = new Texture();

    if(!(picTexture->createTexture()))
    {
        LOGE("create texture failed");
        destroy();
        return false;
    }

//    updateTexImage();

    picRender = new Render();
    if(!picRender->init(width, height, picTexture))
    {
        LOGE("init render failed");
        destroy();
        return false;
    }
    return true;
}

void PicPreviewer::destroy() {
    LOGD("destroy");
    if(eglCore)
    {
        eglCore->releaseSurface(previewSurface);
        eglCore->release();
        delete(eglCore);
        eglCore = NULL;
    }
}

void PicPreviewer::updateTexImage() {
    LOGD("updateTexImage");
//    color += 0x0f;
//    color &= 0x00ffffff;
//    LOGD("color = %x", color);
//    int width = 500;
//    int height = 500;
//    uint8_t *pixel = (uint8_t *)malloc(width * height * 3 * sizeof(uint8_t));
//    for(int i = 0; i < width * height; i++)
//    {
//
//        pixel[3 * i] = (uint8_t)((color >> 16) & 0x00ff);
//        pixel[3 * i + 1] = (uint8_t)((color >> 8) & 0x00ff);
//        pixel[3 * i + 2] = (uint8_t)(color & 0x00ff);
//
//    }

    if(provider == NULL)
    {
        return;
    }
    VideoFrame *f = provider->getVideoFrame();
    picTexture->updateDataToTexture(f->data, f->width, f->height);
//    free(pixel);
}

void PicPreviewer::drawFrame() {
    LOGD("drawFrame");
    picRender->render();
    if(!eglCore->swapBuffers(previewSurface))
    {
        LOGE("swap buffers failed");
    }
}




