#include <stdio.h>
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
#include "com_netease_LDNetDiagnoService_LDNetTraceTelnet.h"
#include "Provider.h"
#define OUT_LEN 1000

JNIEnv* jniEnv;
int isFirst;
extern char* jstringTostring(JNIEnv* env, jstring jstr);
extern int printf(const char *fmt, ...);

JNIEXPORT void JNICALL Java_com_netease_LDNetDiagnoService_LDNetTraceTelnet_startJNICTelnet(JNIEnv *env, jobject obj, jstring command, jstring port){
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNICTelnet begin...." );
	if(jniEnv == NULL) {
		jniEnv = env;
	}

	isFirst = 1;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNICTelnet c_command begin...." );
	char* c_command = jstringTostring(env, command);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNICTelnet c_command end...." );
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNICTelnet c_port begin...." );
	char* c_port = jstringTostring(env, port);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "startJNICTelnet c_port end...." );
	char* argv[]={"connect", c_command, c_port};
	mainConnect(3, argv);
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

void connectHost(struct hostent *server, int portno)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	//struct hostent *server;
	//char buffer[256];
	//char hname[128];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("connect to host failed. ERROR opening socket\n");
		return;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)(&serv_addr.sin_addr.s_addr),
	      server->h_length);
	serv_addr.sin_port = htons(portno);

	time_t begin, end;
	begin = clock();
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("connect to host failed");
		return ;
	}
	end = clock();
	printf("connect to host...success.  %.5f ms", (double)(end-begin)/CLOCKS_PER_SEC *1000 );

	close(sockfd);
}


int mainConnect (int argc, char* argv[])
{
	int portno;
	struct hostent *server;
	int iter;

	if (argc < 3)
	{
		//printf( "usage %s hostname port\n", argv[0]);
		printf("connect to host failed, argument mismatch");
		return 0;
	}
	
	/*获取本机ip地址*/
	printf("local host IP: %s", GetLocalIp());
	portno = atoi(argv[2]);
	/*打印远程主机的ip*/
	printf("\nremote host ip: ");
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		printf("\n connect to host failed, gethostbyname return null \n");
		return 0;
	}
    for(iter = 0; server->h_addr_list[iter]; iter++) {
        printf("%s\t", inet_ntoa(*(struct in_addr*)(server->h_addr_list[iter])));
    }

    printf("\nstart connect... ");
    for(iter=0; iter<3; iter++)
    {
    	printf("\n%d's try,", iter+1);
        connectHost(server, portno);
    }
    printf(" ************ \n\n\n");

	return 0;
}

