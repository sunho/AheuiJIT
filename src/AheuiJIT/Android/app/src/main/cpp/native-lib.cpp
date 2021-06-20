#include <AheuiJIT/Runtime/Runtime.h>
#include <android/log.h>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <string>

static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

static void *thread_func(void *) {
    ssize_t rdsz;
    char buf[128];
    while ((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if (buf[rdsz - 1] == '\n')
            --rdsz;
        buf[rdsz] = 0; /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
    }
    return 0;
}

int start_logger(const char *app_name) {
    tag = app_name;

    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if (pthread_create(&thr, 0, thread_func, 0) == -1)
        return -1;
    pthread_detach(thr);
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
Java_kim_sunho_aheuijitdemo_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
    start_logger(tag);
    aheuijit::Runtime rt(aheuijit::creataeStdioMachine());
    auto conf = rt.getConfig();
    conf.numInterpretCycle = 0;
    conf.logDisasm = false;
    conf.logIR = false;
    conf.optIR = false;
    rt.setConfig(conf);
    auto code = aheuijit::covnert_utf8_to_utf16("삼바반빠빠빠빠빠빠아아우\n우어어번벋범벌벖벍벓벒석"
                                                "\n삭싿삳빠빠빠빠빠빠빠빠술\n초섬어어어어쑫썽뻐선썽"
                                                "부\n숭밝밦따뭏오삳따쌍술뽀분\n망속숙멍성오어어어차"
                                                "솓뿌\n송어밢밦따밝다맣상뭉노뿌\nㄹ모뻐희셩멓떠벌번"
                                                "선뽀뿌\n최종찬만듦밦발밤받반뽀뿌\n유덕남고침볽벓벒"
                                                "선뻐뻐뻐");
    rt.run(code);
    return env->NewStringUTF("hi");
}