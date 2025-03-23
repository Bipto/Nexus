#include <android_native_app_glue.h>
#include <jni.h>
#include <android/log.h>

extern "C" {
    void handle_cmd(android_app* pApp, int32_t cmd)
    {
        switch (cmd)
        {
            case APP_CMD_INIT_WINDOW:
            {
                __android_log_print(ANDROID_LOG_ERROR, "TRACKERS", "Window created");
                break;
            }
            case APP_CMD_TERM_WINDOW:
            {
                __android_log_print(ANDROID_LOG_ERROR, "TRACKERS", "Window destroyed");
                break;
            }
        }
    }
}

void android_main(struct android_app* pApp)
{
    pApp->onAppCmd = handle_cmd;

    int events;
    android_poll_source *pSource;
    do {
        if (ALooper_pollOnce(0, nullptr, &events, (void **) &pSource) >= 0)
        {
            if (pSource)
            {
                pSource->process(pApp, pSource);
            }
        }
    } while (!pApp->destroyRequested);
}