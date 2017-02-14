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
static char g_logDir[PATHLEN] = { 0 };
static char g_fileNameKey[PATHLEN] = { 0 };
static FILE* g_log_file = stderr;


//fileNameKey日志文件名称的关键字，不允许有-减号字符， 否则清理日志功能会出错
//windows各种预定义目录可以通过SHGetSpecialFolderPath获取
///char path[MAX_PATH + 1] = { 0 };
///SHGetSpecialFolderPathA(NULL, path, CSIDL_APPDATA, 0);
int open_log_file(const char* dir, const char* fileNameKey)
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

    strcpy(g_logDir, dir);
    strcpy(g_fileNameKey, fileNameKey);
    time_t currTime;
    struct tm currTm;
    time(&currTime);

    char log_file_name[PATHLEN] = { 0 };
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    localtime_s(&currTm, &currTime);
    sprintf(log_file_name, "%s\\%s-%04d-%02d-%02d-%02d-%02d-%02d.log", dir, fileNameKey,
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);
#else
    localtime_r(&currTime, &currTm);
    sprintf(log_file_name, "%s/%s-%04d-%02d-%02d-%02d-%02d-%02d.log", dir, fileNameKey,
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);
#endif

    g_log_file = fopen(log_file_name, "ab+");//输出位置
    if (NULL == g_log_file)
    {
        return -2;
    }

    return 0;
}

int close_log_file()
{
    if (stderr != g_log_file)
    {
        if (NULL != g_log_file)
        {
            fclose(g_log_file);
            g_log_file = NULL;
        }
    }

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
    int num = sscanf(date, "%04d-%02d-%02d-%02d-%02d-%02d", &begin.tm_year, &begin.tm_mon, &begin.tm_mday,
        &begin.tm_hour, &begin.tm_min, &begin.tm_sec);
    if (6 != num)  return false;
    begin.tm_year -= 1900;
    begin.tm_mon -= 1;
    time_t beginSecond = mktime(&begin);
    if (-1 == beginSecond)  return false;
    time_t endSecond = 0;
    time(&endSecond);
    if ((endSecond - beginSecond) / 3600 / 24 >= days) { return true; }
    return false;
}

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
int CCustomTraverseDir::FileOperation(const TCHAR* file)
{
    const TCHAR* r =  _tcsrchr(file, L'\\');
    if (NULL == r)  { return 0; }
    const TCHAR* p = _tcschr(r, L'-');
    if (NULL == p)  { return 0; }
    const TCHAR* q = _tcschr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }

#if defined(_UNICODE) || defined(UNICODE)
    TCHAR fileNameKey[PATHLEN] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, g_fileNameKey, -1, fileNameKey, PATHLEN);
    if ((p - r - 1) != _tcslen(fileNameKey))  return 0;
    if (wmemcmp(r + 1, fileNameKey, p - r - 1) != 0)  return 0;
#else
    if ((p - r - 1) != strlen(g_fileNameKey))  return 0;
    if (memcmp(r + 1, g_fileNameKey, p - r - 1) != 0)  return 0;
#endif


    TCHAR date[PATHLEN] = { 0 };
    int i = 0;
    p++;
    while (p != q)
    {
        date[i++] = *(p++);
    }
    date[i] = 0;

#if defined(_UNICODE) || defined(UNICODE)
    char date1[PATHLEN] = { 0 };
    WideCharToMultiByte(CP_ACP, 0, date, -1, date1, PATHLEN, 0, 0);
    if (IsDelLog(date1, m_days))
#else
    if (IsDelLog(date, m_days))
#endif
    {
        DeleteFile(file);
    }
    return 0;
}
#else
int CCustomTraverseDir::FileOperation(const char* file)
{
    const char* r =  strrchr(file, L'/');
    if (NULL == r)  { return 0; }
    const char* p = strchr(r, L'-');
    if (NULL == p)  { return 0; }
    const char* q = strchr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }

    if ((p - r - 1) != strlen(g_fileNameKey))  return 0;
    if (memcmp(r + 1, g_fileNameKey, p - r - 1) != 0)  return 0;

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

int clear_log(const unsigned int days)
{
    CCustomTraverseDir TraverseDir(days);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#if defined(_UNICODE) || defined(UNICODE)
    TCHAR logDir1[PATHLEN] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, g_logDir, -1, logDir1, PATHLEN);
    return TraverseDir.TraverseDir(logDir1);
#else
    return TraverseDir.TraverseDir(g_logDir);
#endif
#else
    return TraverseDir.TraverseDir(g_logDir);
#endif
}

//基础log函数  输出： [年-月-日-时-分-秒]文件-行号-函数名称:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...)
{
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    int i = GetLastError();
    time_t currTime;
    struct tm currTm;
    time(&currTime);
    localtime_s(&currTm, &currTime);
    LPVOID lpMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, i, 0, (LPSTR)&lpMsgBuf, 0, NULL);
    fprintf(g_log_file, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s:\r\nSYS_ERRNO: %sUSER: ",
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec,
        fileName, line, funcName, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
    ///SetLastError(0);
#else
    char buf[1024];
    char* p = strerror_r(errno, buf, sizeof(buf));//线程安全版本，buf不一定有值
    time_t currTime;
    struct tm currTm;
    time(&currTime);
    localtime_r(&currTime, &currTm);
    fprintf(g_log_file, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s:\nSYS_ERRNO: %s\nUSER: ",
        currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec,
        fileName, line, funcName, p);
#endif

    va_list ap;
    va_start(ap, fmt);
    vfprintf(g_log_file, fmt, ap);
    va_end(ap);

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    fprintf(g_log_file, "\r\n\r\n");
#else
    fprintf(g_log_file, "\n\n");
#endif

    fflush(g_log_file);

    return 0;
}