#include "log.h"
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
#pragma warning(disable: 4267)


static char logFile[266] = {0};


void setLogFile(const char* file)
{
	strcpy(logFile, file);
}


//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log(const char* fileName, int line, const char* funcName, const char *fmt, ...)
{
	FILE* fp = NULL;
	time_t currTime;
	struct tm currTm;
	va_list ap;

	if (0 == logFile[0])
	{
		fp = stderr;
	}
	else
	{
		fp = fopen(logFile, "ab+");//���λ��
		if (NULL == fp)
		{
			return -1;
		}
	}

	time(&currTime);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
	localtime_s(&currTm, &currTime);
	fprintf(fp, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s:\r\n",
		currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec,
		fileName, line, funcName);
#else
	localtime_r(&currTime, &currTm);
	fprintf(fp, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s:\n",
		currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec,
		fileName, line, funcName);
#endif

	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
	fprintf(fp, "\r\n");
#else
	fprintf(fp, "\n");
#endif

	if (0 != logFile[0])
	{
		fclose(fp);
	}
	
	return 0;
}


//�Զ���log����1  ����log������� + syserrno:ϵͳ��������,userDescription:�Զ�������
int _log1(const char* fileName, int line, const char* funcName, const char* userDescription)
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, (LPSTR)&lpMsgBuf, 0, NULL);
	_log(fileName, line, funcName, "syserrno:%suserDescription:%s\r\n", (char*)lpMsgBuf, userDescription);
	LocalFree(lpMsgBuf);
#else
	char buf[1024];
	char* p = strerror_r(errno, buf, sizeof(buf));//�̰߳�ȫ�汾��buf��һ����ֵ
	_log(fileName, line, funcName, "syserrno:%s\nuserDescription:%s\n", p, userDescription);
#endif

	return 0;
}
