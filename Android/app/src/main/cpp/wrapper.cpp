#include <jni.h>
#include <string>
#include <memory>
#include <android/log.h>

#define APP_NAME "BallRoller"

#include "../../../../../BallRollerEngine/Engine.h"
#include "../../../../../BallRollerEngine/Device.h"

class CAndroidDevice : public IDevice {
public:
    void Log(const std::string& text) override {
        __android_log_print(ANDROID_LOG_VERBOSE, APP_NAME, "%s", text.c_str());
    }
};

std::unique_ptr<CAndroidDevice> g_pDevice;
std::unique_ptr<CEngine> g_pEngine;


extern "C" {
    jboolean Java_coderulers_info_ballroller_Engine_Initialize(JNIEnv *, jobject /* this */) {
        if(!g_pEngine) {
            g_pDevice = std::unique_ptr<CAndroidDevice>(new CAndroidDevice());
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