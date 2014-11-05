#include "Provider.h"

extern JNIEnv* jniEnv;
extern int isFirst;

jclass TestProvider;
jobject mTestProvider;
jmethodID printTraceInfo;

/**
 * 初始化 类、对象、方法
 */
int InitProvider() {
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  1 ok" );
	if(jniEnv == NULL) {
		return 0;
	}

	if(TestProvider == NULL) {
		TestProvider = (*jniEnv)->FindClass(jniEnv,"com/netease/LDNetDiagnoService/LDNetTraceRoute");
		if(TestProvider == NULL){
			return -1;
		}
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  2 ok" );
	}

	if (mTestProvider == NULL) {
		jfieldID fid = (*jniEnv)->GetStaticFieldID(jniEnv, TestProvider, "instance", "Lcom/netease/LDNetDiagnoService/LDNetTraceRoute;");
		if(fid == NULL){
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get static object id error" );
			return -2;
		}

		mTestProvider = (*jniEnv)->GetStaticObjectField(jniEnv, TestProvider, fid);
		if (mTestProvider == NULL) {
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get instance 33error" );
			return -2;
		}

		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get instance ok" );
	}

	if (printTraceInfo == NULL) {
		printTraceInfo = (*jniEnv)->GetMethodID(jniEnv, TestProvider, "printTraceInfo","(Ljava/lang/String;)V");
		if (printTraceInfo == NULL) {
			(*jniEnv)->DeleteLocalRef(jniEnv, TestProvider);
			(*jniEnv)->DeleteLocalRef(jniEnv, mTestProvider);
			return -2;
		}
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  4 ok" );
	}

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  5 ok" );
	return 1;
}



/**
 * SayHello ---- 调用 Java 方法
 */
void PrintTraceInfo(const char *aStrToPrint) {
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "printf result begin...." );
	if(isFirst == 1) {
		TestProvider = NULL;
		mTestProvider = NULL;
		printTraceInfo = NULL;
//	if(TestProvider == NULL || mTestProvider == NULL || printTraceInfo == NULL) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "enter the printTraceInfo" );
		int result = InitProvider() ;
		if(result != 1) {
			return;
		}
	}

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "printf call printTrackInfo begin...." );
	jstring jstrMSG = NULL;
	//jstrMSG =(*jniEnv)->NewStringUTF(jniEnv, "Hi,I'm From C");
	jstrMSG =(*jniEnv)->NewStringUTF(jniEnv, aStrToPrint);
	(*jniEnv)->CallVoidMethod(jniEnv, mTestProvider, printTraceInfo,jstrMSG);
	(*jniEnv)->DeleteLocalRef(jniEnv, jstrMSG);
}
