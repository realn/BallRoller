#include <jni.h>
#include <string>
#include <memory>
#include <android/log.h>

#define APP_NAME "BallRoller"

#include "../../../../../BallRollerEngine/Engine.h"
#include "../../../../../BallRollerEngine/Device.h"

class CAndroidDevice : public IDevice {
private:
    JNIEnv* mEnv;
    jobject mPngMng;

public:
    CAndroidDevice(JNIEnv* env, jobject pngMng) : mEnv(env), mPngMng(pngMng){}

    void Log(const std::string& text) override {
        __android_log_print(ANDROID_LOG_VERBOSE, APP_NAME, "%s", text.c_str());
    }
    bool LoadPng(const std::string& name, std::vector<int>& outData, int& outWidth, int& outHeight){
        const std::string path = "images/" + name + ".png";

        jclass cls = mEnv->GetObjectClass(mPngMng);
        jmethodID mid;

        /* Ask the PNG manager for a bitmap */
        mid = mEnv->GetMethodID(cls, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        jstring javaPath = mEnv->NewStringUTF(path.c_str());
        jobject png = mEnv->CallObjectMethod(mPngMng, mid, name);
        mEnv->DeleteLocalRef(javaPath);
        if(png == nullptr)
            return false;
        mEnv->NewGlobalRef(png);

        /* Get image dimensions */
        mid = mEnv->GetMethodID(cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
        outWidth = mEnv->CallIntMethod(mPngMng, mid, png);
        mid = mEnv->GetMethodID(cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
        outHeight = mEnv->CallIntMethod(mPngMng, mid, png);

        int dataSize = outWidth * outHeight;
        /* Get pixels */
        jintArray array = mEnv->NewIntArray(dataSize);
        mEnv->NewGlobalRef(array);
        mid = mEnv->GetMethodID(cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
        mEnv->CallVoidMethod(mPngMng, mid, png, array);

        jint *pixels = mEnv->GetIntArrayElements(array, 0);
        outData.insert(outData.end(), pixels, pixels + dataSize);

        mEnv->ReleaseIntArrayElements(array, pixels, 0);
        mEnv->DeleteGlobalRef(array);

        /* Free image */
        mid = mEnv->GetMethodID(cls, "close", "(Landroid/graphics/Bitmap;)V");
        mEnv->CallVoidMethod(mPngMng, mid, png);
        mEnv->DeleteGlobalRef(png);
        mEnv->DeleteLocalRef(cls);
        return true;
    }
};

std::unique_ptr<CAndroidDevice> g_pDevice;
std::unique_ptr<CEngine> g_pEngine;


extern "C" {
    jboolean Java_coderulers_info_ballroller_Engine_Initialize(JNIEnv *env, jobject /* this */, jobject pngMng) {
        if(!g_pEngine) {
            g_pDevice = std::unique_ptr<CAndroidDevice>(new CAndroidDevice(env, pngMng));
            g_pEngine = std::unique_ptr<CEngine>(new CEngine(g_pDevice.get()));
        }

        bool result = g_pEngine->Initialize();
        return result;
    }

    void Java_coderulers_info_ballroller_Engine_ScreenChanged(JNIEnv *, jobject, jint width, jint height) {
        g_pEngine->ScreenChanged(width, height);
    }

    void Java_coderulers_info_ballroller_Engine_FrameUpdate(JNIEnv *, jobject, jfloat timeDelta) {
        g_pEngine->FrameUpdate(timeDelta);
    }

    void Java_coderulers_info_ballroller_Engine_Release(JNIEnv *, jobject) {
        g_pEngine->Release();
    }
}