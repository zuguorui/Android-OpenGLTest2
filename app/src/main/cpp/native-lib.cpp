#include <jni.h>
#include <string>
#include <iostream>
#include <stdlib.h>

#include <android/log.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include "ColorFrameProvider.h"

using namespace std;

#define MODULE_NAME "native_lib"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

ColorFrameProvider *provider = NULL;
ANativeWindow *window = NULL;

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nInit(JNIEnv *env, jobject instance)
{
    provider = new ColorFrameProvider();
}

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nDestroy(JNIEnv *env, jobject instance)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nDestroy");
        return;
    }
    delete(provider);
}

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nStart(JNIEnv *env, jobject instance)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nStart");
        return;
    }
    provider->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nStop(JNIEnv *env, jobject instance)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nStop");
        return;
    }
    provider->stop();
}





extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nSetSurface(JNIEnv *env, jobject instance, jobject surface)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nSetSurface");
        return;
    }
    window = ANativeWindow_fromSurface(env, surface);
    provider->setSurface(window);
}

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nReleaseSurface(JNIEnv *env, jobject instance, jobject surface)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nReleaseSurface");
        return;
    }
    if(window != NULL)
    {
        ANativeWindow_release(window);
        window = NULL;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_zu_opengltest2_MainActivity_nSetSize(JNIEnv *env, jobject instance, jint width, jint height)
{
    if(provider == NULL)
    {
        LOGE("controller is NULL when call nSetSize");
        return;
    }
    provider->setSize(width, height);
}




