#include "Provider.h"

extern int isFirst;
extern JNIEnv* jniEnvTelnet;
extern int isFirstTelnet;

JavaVM *gJvm;
jclass TestProvider;
jobject mTestProvider;
jmethodID printTraceInfo;

jclass _LDNetSocket;
jobject _mLDNetSocket;
jmethodID printSocketInfo;

/**
 * 初始化 类、对象、方法
 */
int InitProvider(JNIEnv *jniEnv) {
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
 * 初始化 类、对象、方法
 */
int InitSocketInfo() {
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitSocketInfo Begin  1 ok" );
	if(jniEnvTelnet == NULL) {
		return 0;
	}

	if(_LDNetSocket == NULL) {
		_LDNetSocket = (*jniEnvTelnet)->FindClass(jniEnvTelnet,"com/netease/LDNetDiagnoService/LDNetSocket");
		if(_LDNetSocket == NULL){
			return -1;
		}
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  2 ok" );
	}

	if (_mLDNetSocket == NULL) {
		jfieldID fid = (*jniEnvTelnet)->GetStaticFieldID(jniEnvTelnet, _LDNetSocket, "instance", "Lcom/netease/LDNetDiagnoService/LDNetSocket;");
		if(fid == NULL){
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get static object id error" );
			return -2;
		}

		_mLDNetSocket = (*jniEnvTelnet)->GetStaticObjectField(jniEnvTelnet, _LDNetSocket, fid);
		if (_mLDNetSocket == NULL) {
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get instance 33error" );
			return -2;
		}

		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin get instance ok" );
	}

	if (printSocketInfo == NULL) {
		printSocketInfo = (*jniEnvTelnet)->GetMethodID(jniEnvTelnet, _LDNetSocket, "printSocketInfo","(Ljava/lang/String;)V");
		if (printSocketInfo == NULL) {
			(*jniEnvTelnet)->DeleteLocalRef(jniEnvTelnet, _LDNetSocket);
			(*jniEnvTelnet)->DeleteLocalRef(jniEnvTelnet, _mLDNetSocket);
			return -2;
		}
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  4 ok" );
	}

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "InitProvider Begin  5 ok" );
	return 1;
}

void PrintSocketInfo(const char *aStrToPrint){
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "PrintSocketInfo begin...." );
	if(isFirstTelnet == 1) {
		_LDNetSocket = NULL;
		_mLDNetSocket = NULL;
		printSocketInfo = NULL;
//	if(TestProvider == NULL || mTestProvider == NULL || printTraceInfo == NULL) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "enter the printSocketInfo" );
		int result = InitSocketInfo() ;
		if(result != 1) {
			return;
		}
	}

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "printf call printSocketInfo begin...." );
	jstring jstrMSG = NULL;
	//jstrMSG =(*jniEnv)->NewStringUTF(jniEnv, "Hi,I'm From C");
	jstrMSG =(*jniEnvTelnet)->NewStringUTF(jniEnvTelnet, aStrToPrint);
	(*jniEnvTelnet)->CallVoidMethod(jniEnvTelnet, _mLDNetSocket, printSocketInfo,jstrMSG);
	//(*jniEnvTelnet)->DeleteLocalRef(jniEnvTelnet, jstrMSG);
}

/**
 * SayHello ---- 调用 Java 方法
 */
void PrintTraceInfo(const char *aStrToPrint) {
	//获取当前线程的jniEnv
	JNIEnv *jniEnv;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "get jniEnv from currentThread 1...." );
	(*gJvm)->GetEnv(gJvm, (void **)&jniEnv, JNI_VERSION_1_6);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "get jniEnv from currentThread 2...." );
	if(jniEnv == NULL){
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "get jniEnv from currentThread null...." );
	} else {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "get jniEnv from currentThread success...." );
	}

	if(isFirst == 1) {
		TestProvider = NULL;
		mTestProvider = NULL;
		printTraceInfo = NULL;
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "init the provider info...." );
		int result = InitProvider(jniEnv);
		if(result != 1) {
			return;
		}
	}

	if(mTestProvider != NULL && printTraceInfo != NULL) {
		jstring jstrMSG = NULL;
		jstrMSG =(*jniEnv)->NewStringUTF(jniEnv, aStrToPrint);
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "call java printTrackInfo begin...." );
		(*jniEnv)->CallVoidMethod(jniEnv, mTestProvider, printTraceInfo,jstrMSG);
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "call java printTrackInfo after...." );
		(*jniEnv)->DeleteLocalRef(jniEnv, jstrMSG);
	}
}
