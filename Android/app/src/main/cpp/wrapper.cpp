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
    bool LoadAsset(const std::string& name, std::vector<unsigned char>& outData) override {
        const std::string path = name;

        jclass cls = mEnv->GetObjectClass(mPngMng);
        jmethodID mid = mEnv->GetMethodID(cls, "open", "(Ljava/lang/String;)[B");
        jstring javaPath = mEnv->NewStringUTF(path.c_str());

        jbyteArray javaByteArray = (jbyteArray)mEnv->CallObjectMethod(mPngMng ,mid, javaPath);
        if(javaByteArray == nullptr)
            return false;

        int len = mEnv->GetArrayLength(javaByteArray);
        if(len == 0) {
            mEnv->DeleteLocalRef(javaByteArray);
            return false;
        }
        outData.resize(len);
        mEnv->GetByteArrayRegion(javaByteArray, 0, len, (jbyte*)&outData[0]);
        mEnv->DeleteLocalRef(javaByteArray);
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

    void Java_coderulers_info_ballroller_Engine_UserAction(JNIEnv *, jobject, jint action) {
        g_pEngine->UserAction((UserActionType)action);
    }

    void Java_coderulers_info_ballroller_Engine_Release(JNIEnv *, jobject) {
        g_pEngine->Release();
    }
}