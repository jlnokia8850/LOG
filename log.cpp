/*
���ߣ����
QQ:   243660565
*/
#ifndef ___LOG___
#define ___LOG___

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <windows.h>
#else
#include <errno.h>
#endif

#pragma warning(disable: 4996)


//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log(char* fileName, int line, char* funcName, char *fmt, ...)
{
	FILE* fp = NULL;
	time_t currTime;
	struct tm currTm;
	va_list ap;
	const char* logFile = "log.txt"; //��־����ļ�


#if 1
	fp = fopen(logFile, "a+");//���λ��
	if (NULL == fp)
	{
		return -1;
	}
#endif

#if 0
	fp = stderr;  //���λ��
#endif

	time(&currTime);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
	localtime_s(&currTm, &currTime);
#else
	localtime_r(&currTime, &currTm);
#endif

	fprintf(fp, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s:\n",
		currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec,
		fileName, line, funcName);

	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);

	fprintf(fp, "\n");
	fclose(fp);
	return 0;
}


//�Զ���log����1  ����log������� + syserrno:ϵͳ��������,userDescription:�Զ�������
int _log1(char* fileName, int line, char* funcName, char* userDescription = "")
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, (LPSTR)&lpMsgBuf, 0, NULL);
	char* p = (char*)lpMsgBuf;
	int len = strlen(p);
	if (len > 0)
	{
		p[len - 1] = 0;   //ȥ�����з�
	}	
	_log(fileName, line, funcName, "syserrno:%s\nuserDescription:%s\n", lpMsgBuf, userDescription);
	LocalFree(lpMsgBuf);
#else
	char buf[1024];
	char* p = strerror_r(errno, buf, sizeof(buf));//�̰߳�ȫ�汾��buf��һ����ֵ
	_log(fileName, line, funcName, "syserrno:%s\nuserDescription:%s\n", p, userDescription);
#endif

	return 0;
}
#endif
