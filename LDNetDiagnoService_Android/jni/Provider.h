#include <string.h>
#include <jni.h>
#include <pthread.h>
#include <android/log.h>

static pthread_mutex_t mutex;

void PrintTraceInfo(const char *aStrToPrint);

void PrintSocketInfo(const char *aStrToPrint);
