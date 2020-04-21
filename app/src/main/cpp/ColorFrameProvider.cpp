//
// Created by 祖国瑞 on 2020-04-20.
//

#include "ColorFrameProvider.h"
#include <android/log.h>
#include <chrono>


#define MODULE_NAME "ColorFrameProvider"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

using namespace std;

ColorFrameProvider::ColorFrameProvider() {
    queue = new BlockRecyclerQueue<VideoFrame *>(5);
    player = new OpenGLESPlayer2();
    if(!player->create())
    {
        LOGE("player create failed");
    }
    player->setVideoFrameProvider(this);
//    player->setVideoFrameProvider(this);

}

ColorFrameProvider::~ColorFrameProvider() {
//    player->removeVideoFrameProvider(this);
//    player->release();
    player->release();
    queue->discardAll(NULL);
    VideoFrame *f = NULL;
    while((f = queue->get(false)) != NULL)
    {
        delete(f);
        f = NULL;
    }
    while((f = queue->getUsed()) != NULL)
    {
        delete(f);
        f = NULL;
    }
    delete(queue);
}

void ColorFrameProvider::start() {
    stopFlag = false;
    generateThread = new thread(threadCallback, this);
}

void ColorFrameProvider::stop() {
    stopFlag = true;
    if(generateThread != NULL && generateThread->joinable())
    {
        generateThread->join();
    }
}

void ColorFrameProvider::setSurface(void *surface) {
    player->setWindow((ANativeWindow *)surface);
}

void ColorFrameProvider::setSize(int32_t width, int32_t height) {
    player->setSize(width, height);
}

void* ColorFrameProvider::threadCallback(void *self) {
    ColorFrameProvider *provider = (ColorFrameProvider *)self;
    provider->generateLoop();
}

void ColorFrameProvider::generateLoop() {
    int32_t dataSizeInByte = HEIGHT * WIDTH * 3;
    color[0] = 0xff;
    color[1] = 0xff;
    color[2] = 0x00;
    int colorIndex = 1;
    while(!stopFlag)
    {
        int prevIndex = (colorIndex - 1 + 3) % 3;
        int nextIndex = (colorIndex + 1) % 3;
        if(color[prevIndex] != 0)
        {
            color[prevIndex] += 1;
        }
        if(color[nextIndex] != (uint8_t)0xff)
        {
            color[nextIndex] += 1;
        }
        if(color[prevIndex] == 0 && color[nextIndex] == (uint8_t)0xff)
        {
            colorIndex = (colorIndex + 1) % 1;
        }

        VideoFrame *f = queue->getUsed();
        if(f == NULL)
        {
            f = new VideoFrame(dataSizeInByte);
            f->width = WIDTH;
            f->height = HEIGHT;
        }
        for(int i = 0; i < WIDTH * HEIGHT; i++)
        {
            f->data[3 * i] = color[0];
            f->data[3 * i + 1] = color[1];
            f->data[3 * i + 2] = color[2];
        }
        queue->put(f);
        player->refresh();
        this_thread::sleep_for(chrono::milliseconds(20));
    }
}


VideoFrame *ColorFrameProvider::getVideoFrame() {
    return queue->get();
}

void ColorFrameProvider::putBackUsed(VideoFrame *data) {
    queue->putToUsed(data);
}
