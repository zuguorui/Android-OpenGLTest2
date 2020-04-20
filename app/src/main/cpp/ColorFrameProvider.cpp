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
    player = new PicPreviewer();
    if(!player->start())
    {
        LOGE("player create failed");
    }
    player->setProvider(this);
//    player->setVideoFrameProvider(this);

}

ColorFrameProvider::~ColorFrameProvider() {
//    player->removeVideoFrameProvider(this);
//    player->release();
    player->stop();
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
    player->resetSize(width, height);
}

void* ColorFrameProvider::threadCallback(void *self) {
    ColorFrameProvider *provider = (ColorFrameProvider *)self;
    provider->generateLoop();
}

void ColorFrameProvider::generateLoop() {
    int32_t dataSizeInByte = HEIGHT * WIDTH * 3;
    while(!stopFlag)
    {
        color += 0x00010101;
        color &= 0x00ffffff;
        VideoFrame *f = queue->getUsed();
        if(f == NULL)
        {
            f = new VideoFrame(dataSizeInByte);
            f->width = WIDTH;
            f->height = HEIGHT;
        }
        for(int i = 0; i < WIDTH * HEIGHT; i++)
        {
            f->data[3 * i] = (uint8_t)((color >> 16) & 0x00ff);
            f->data[3 * i + 1] = (uint8_t)((color >> 8) & 0x00ff);
            f->data[3 * i + 2] = (uint8_t)(color & 0x00ff);
        }
        queue->put(f);

        player->changeColor();
        this_thread::sleep_for(chrono::milliseconds(40));
    }
}


VideoFrame *ColorFrameProvider::getVideoFrame() {
    return queue->get();
}

void ColorFrameProvider::putBackUsed(VideoFrame *data) {
    queue->putToUsed(data);
}
