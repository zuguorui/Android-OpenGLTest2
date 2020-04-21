//
// Created by 祖国瑞 on 2020-04-20.
//

#ifndef ANDROID_OPENGLTEST2_COLORFRAMEPROVIDER_H
#define ANDROID_OPENGLTEST2_COLORFRAMEPROVIDER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

#include "IVideoFrameProvider.h"
#include "IVideoPlayer.h"
#include "BlockRecyclerQueue.h"
#include "OpenGLESPlayer2.h"
#include "PicPreviewer.h"



using namespace std;

class ColorFrameProvider: public IVideoFrameProvider {
public:

    ColorFrameProvider();
    ~ColorFrameProvider();

    void start();
    void stop();

    void setSurface(void *surface);

    void setSize(int32_t width, int32_t height);


    VideoFrame *getVideoFrame() override;

    void putBackUsed(VideoFrame *data) override;

private:
    uint8_t color[3];
    BlockRecyclerQueue<VideoFrame *> *queue = NULL;

    OpenGLESPlayer2 *player = NULL;

    static void *threadCallback(void *self);

    void generateLoop();

    thread *generateThread = NULL;

    bool stopFlag = 0;



    static const int32_t WIDTH = 500;
    static const int32_t HEIGHT = 500;
};


#endif //ANDROID_OPENGLTEST2_COLORFRAMEPROVIDER_H
