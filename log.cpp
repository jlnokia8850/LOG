#include "stdafx.h"
#include "log.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "CTraverseDir.h"
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <io.h>
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif

#pragma warning(disable: 4996)
#pragma warning(disable: 4267)

static const int PATHLEN = 512;
static char logDir[PATHLEN] = { 0 };
static char logFile[PATHLEN] = { 0 };

int SetLogDir(const char* dir)
{
    if (access(dir, 0) < 0)
    {
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
        if (mkdir(dir) < 0)
#else
        if (mkdir(dir, 0777) < 0)
#endif
        {
            return -1;
        }
    }

    strcpy(logDir, dir);
    time_t currTime;
    struct tm currTm;
    time(&currTime);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    localtime_s(&currTm, &currTime);
    sprintf(logFile, "%s\\%04d-%02d-%02d-%02d-%02d-%02d.log", dir,
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);
#else
    localtime_r(&currTime, &currTm);
    sprintf(logFile, "%s/%04d-%02d-%02d-%02d-%02d-%02d.log", dir,
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);
#endif

    return 0;
}

class CCustomTraverseDir : public CTraverseDir
{
public:
    CCustomTraverseDir(const unsigned int days) : m_days(days) {}
private:
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    virtual int FileOperation(const TCHAR* file);
#else
    virtual int FileOperation(const char* file);
#endif
    unsigned int m_days;
};

static bool IsDelLog(const char* date, const unsigned int days)
{
    struct tm begin = { 0 };
    sscanf(date, "%04d-%02d-%02d-%02d-%02d-%02d", &begin.tm_year, &begin.tm_mon, &begin.tm_mday,
        &begin.tm_hour, &begin.tm_min, &begin.tm_sec);
    begin.tm_year -= 1900;
    begin.tm_mon -= 1;
    time_t beginSecond = mktime(&begin);
    time_t endSecond = 0;
    time(&endSecond);
    if ((endSecond - beginSecond) / 3600 / 24 > days) { return true; }
    return false;
}

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
int CCustomTraverseDir::FileOperation(const TCHAR* file)
{
    const TCHAR* p =  _tcsrchr(file, L'\\');
    if (NULL == p)  { return 0; }
    const TCHAR* q = _tcschr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }
    TCHAR date[PATHLEN] = { 0 };
    int i = 0;
    p++;
    while (p != q)
    {
        date[i++] = *(p++);
    }
    date[i] = 0;

    char date1[PATHLEN] = { 0 };
    WideCharToMultiByte(CP_ACP, 0, date, -1, date1, PATHLEN, 0, 0);
    if (IsDelLog(date1, m_days))
    {
        DeleteFile(file);
    }
    return 0;
}
#else
int CCustomTraverseDir::FileOperation(const char* file)
{
    const char* p =  strrchr(file, L'/');
    if (NULL == p)  { return 0; }
    const char* q = strchr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }
    char date[PATHLEN] = { 0 };
    int i = 0;
    p++;
    while (p != q)
    {
        date[i++] = *(p++);
    }
    date[i] = 0;

    if (IsDelLog(date, m_days))
    {
        remove(file);
    }
    return 0;
}
#endif

int ClearLog(const unsigned int days)
{
    CCustomTraverseDir TraverseDir(days);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    TCHAR logDir1[PATHLEN] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, logDir, -1, logDir1, PATHLEN);
    return TraverseDir.TraverseDir(logDir1);
#else
    return TraverseDir.TraverseDir(logDir);
#endif
}

//基础log函数  输出： [年-月-日-时-分-秒]文件-行号-函数名称:
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
        fp = fopen(logFile, "ab+");//输出位置
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


//自定义log函数1  基础log函数输出 + syserrno:系统错误描述,userDescription:自定义描述
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
    char* p = strerror_r(errno, buf, sizeof(buf));//线程安全版本，buf不一定有值
    _log(fileName, line, funcName, "syserrno:%s\nuserDescription:%s\n", p, userDescription);
#endif

    return 0;
}
