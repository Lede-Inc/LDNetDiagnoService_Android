#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <stdarg.h>
#include "com_netease_LDNetDiagnoService_LDNetSocket.h"
#include "Provider.h"
#define OUT_LEN 1000

JavaVM *gTelnetJvm = NULL;
pthread_mutex_t mutexTel = PTHREAD_MUTEX_INITIALIZER;

int isFirstTelnet;
extern char* jstringTostring(JNIEnv* env, jstring jstr);
int Lprintf(const char *fmt, ...);
JNIEXPORT void JNICALL Java_com_netease_LDNetDiagnoService_LDNetSocket_startJNITelnet(JNIEnv *env, jobject obj, jstring command, jstring port){
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNITelnet begin...." );
	(*env)->GetJavaVM(env, &gTelnetJvm);
	(*gTelnetJvm)->AttachCurrentThread(gTelnetJvm, &env, NULL);

	isFirstTelnet = 1;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNITelnet c_command begin...." );
	char* c_command = jstringTostring(env, command);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNITelnet c_command end...." );
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNITelnet c_port begin...." );
	char* c_port = jstringTostring(env, port);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNITelnet c_port end...." );
	char* argv[]={"connect", c_command, c_port};
	mainConnect(3, argv);
}

/**
 * 重载printf函数，将tcpConnect的结果通过PrintTraceInfo返回到java环境
 */
int Lprintf(const char *fmt, ...){
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	char *buffer = (char *)malloc(OUT_LEN);
	memset(buffer, OUT_LEN, 0);
	cnt = vsnprintf(buffer, OUT_LEN, fmt, argptr);
	buffer[cnt] = '\0';
	pthread_mutex_lock(&mutexTel);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "print lock:>>>>>>%d", mutexTel.value);
	PrintSocketInfo(buffer);
	pthread_mutex_unlock(&mutexTel);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "print unlock>>>>>>%d", mutexTel.value);
	free(buffer);
	va_end(argptr);
	isFirstTelnet++;
	return 1;
}

char* GetLocalIp()
{
    int MAXINTERFACES=16;
    char *ip = NULL;
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while (intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    ip=(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        close (fd);
        return ip;
    }
}
//
//void connectHost(struct hostent *server, int portno)
//{
//	int sockfd;
//	struct sockaddr_in serv_addr;
//	//struct hostent *server;
//	//char buffer[256];
//	//char hname[128];
//
//	sockfd = socket(AF_INET, SOCK_STREAM, 0);
//	if (sockfd < 0)
//	{
//		printf("connect to host failed. ERROR opening socket\n");
//		return;
//	}
//
//	memset(&serv_addr, 0, sizeof(serv_addr));
//
//	serv_addr.sin_family = AF_INET;
//	bcopy((char *)server->h_addr, (char *)(&serv_addr.sin_addr.s_addr),
//	      server->h_length);
//	serv_addr.sin_port = htons(portno);
//
//	time_t begin, end;
//	begin = clock();
//	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
//	{
//		printf("connect to host failed");
//		return ;
//	}
//	end = clock();
//	printf("connect to host...success.  %.5f ms", (double)(end-begin)/CLOCKS_PER_SEC *1000 );
//
//	close(sockfd);
//}

void connectHost(struct sockaddr_in serv_addr)
{
	int sockfd;
	time_t begin, end;
	int iter;
	double avg =0;
	double ttime[4] ={0};
	for(iter =0; iter<4; iter++)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			Lprintf("connect to host failed. ERROR opening socket\n");
			return;
		}

		begin = clock();
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			Lprintf("connect to host failed\n");
			return ;
		}
		end = clock();
		ttime[iter]=(double)(end-begin)/CLOCKS_PER_SEC *1000;
		avg+=ttime[iter];
		close(sockfd);
	}
	avg /= 4;
	Lprintf("connect to host %s\n", inet_ntoa(serv_addr.sin_addr));
	Lprintf("1's time:%.0fms, 2's time:%.0fms, 3'time:%.0fms, 4's time:%.0fms, avg time:%.0fms\n",ttime[0],ttime[1],ttime[2],ttime[3],avg);
}

int mainConnect (int argc, char* argv[])
{
	int portno;
	struct hostent *server;
	int iter;
	struct sockaddr_in serv_addr;

	if (argc < 3)
	{
		//printf( "usage %s hostname port\n", argv[0]);
		Lprintf("connect to host failed, argument mismatch\n");
		return 0;
	}

	/*打印本地ip地址*/
	//printf("local host IP: %s", GetLocalIp());
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		Lprintf("\n connect to host failed, gethostbyname return null \n");
		return 0;
	}
    for(iter = 0; server->h_addr_list[iter]; iter++) {
        //Lprintf("%s\t", inet_ntoa(*(struct in_addr*)(server->h_addr_list[iter])));
    	char* maddr =  inet_ntoa(*(struct in_addr*)(server->h_addr_list[iter]));
        serv_addr.sin_addr.s_addr = inet_addr(maddr);
        connectHost(serv_addr);
    }
	return 0;
}

