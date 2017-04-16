#include "log.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>
#include <deque>
using namespace std;
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <dbghelp.h> 
#pragma comment(lib,  "dbghelp.lib")
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif

#pragma warning(disable: 4996)
#pragma warning(disable: 4267)

static const int PATHLEN = 512;
static char g_log_dir[PATHLEN] = { 0 };
static char g_file_name_key[PATHLEN] = { 0 };
static FILE* g_log_file = stderr;
static int g_level = 1;

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
//ֱ��ִ��EXE���ܲ���dmp�ļ�,����pdb�ļ�
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* lpExceptionInfo)
{
    char file_name[MAX_PATH];
    time_t curr_time;
    struct tm curr_tm;
    time(&curr_time);
    localtime_s(&curr_tm, &curr_time);
    sprintf(file_name, "%s\\%s-%04d-%02d-%02d-%02d-%02d-%02d.dmp", g_log_dir, g_file_name_key,
        curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);

    HANDLE hFile = ::CreateFileA(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != hFile)
    {
        MINIDUMP_EXCEPTION_INFORMATION ExInfo;
        ExInfo.ThreadId = ::GetCurrentThreadId();
        ExInfo.ExceptionPointers = lpExceptionInfo;
        ExInfo.ClientPointers = false;

        BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
        if (bOK)  LOGN("MiniDumpWriteDump %s ok", file_name);
        else  LOGE("MiniDumpWriteDump fail");
        ::CloseHandle(hFile);
    }
    else
    {
        LOGE("CreateFile %s fail", file_name);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#else
/*
ulimit -c����鿴core�ļ����ء�Ϊ0����ʾ�ر��˴˹��ܣ���������core�ļ�
ulimit -c filesize������core�ļ��Ĵ�С��filesize�ĵ�λΪkbyte������ulimit -c unlimited����ʾcore�ļ��Ĵ�С�������ơ�
������ɵ���Ϣ�����˴�С������һ����������core�ļ������Դ�core�ļ���ʱ��gdb����ʾ����
ulimit�ڽű�����ִ����Ч������source *.sh����ִ�г���ǰshell��ִ��ulimit -c unlimited

coreĬ��·��Ϊ��ִ���ļ����е�ͬһ·���¡�
��ϵͳ���ɵ�core�ļ����������κ���չ���ƣ���ȫ������Ϊcore���µ�core�ļ����ɽ�����ԭ����core�ļ���
/proc/sys/kernel/core_uses_pid���Կ���core�ļ����ļ������Ƿ����pid��Ϊ��չ���ļ�����Ϊ1����ʾ���pid��Ϊ��չ�������ɵ�core�ļ���ʽΪcore.xxxx��
Ϊ0���ʾ���ɵ�core�ļ�ͬһ����Ϊcore��

proc/sys/kernel/core_pattern���Կ���core�ļ�����λ�ú��ļ�����ʽ��
��ͨ�����������޸Ĵ��ļ���
echo "/corefile/core-%e-%p-%t" > core_pattern�����Խ�core�ļ�ͳһ���ɵ�/corefileĿ¼�£��������ļ���Ϊcore-������-pid-ʱ���
�����ǲ����б�:
%p - insert pid into filename ���pid
%u - insert current uid into filename ��ӵ�ǰuid
%g - insert current gid into filename ��ӵ�ǰgid
%s - insert signal that caused the coredump into the filename ��ӵ��²���core���ź�
%t - insert UNIX time that the coredump occurred into filename ���core�ļ�����ʱ��unixʱ��
%h - insert hostname where the coredump happened into filename ���������
%e - insert coredumping executable name into filename ���������

gdb -core=core.xxxx
file ./a.out
bt
*/
#endif

//windows����Ԥ����Ŀ¼����ͨ��SHGetSpecialFolderPath��ȡ
///char path[MAX_PATH + 1] = { 0 };
///SHGetSpecialFolderPathA(NULL, path, CSIDL_APPDATA, 0);
int open_log_file(const char* dir, const char* file_name_key, const int days)
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

    strcpy(g_log_dir, dir);
    strcpy(g_file_name_key, file_name_key);
    time_t curr_time;
    struct tm curr_tm;
    time(&curr_time);

    char log_file_name[PATHLEN] = { 0 };
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    localtime_s(&curr_tm, &curr_time);
    sprintf(log_file_name, "%s\\%s-%04d-%02d-%02d-%02d-%02d-%02d.log", dir, file_name_key,
        curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
#else
    localtime_r(&curr_time, &curr_tm);
    sprintf(log_file_name, "%s/%s-%04d-%02d-%02d-%02d-%02d-%02d.log", dir, file_name_key,
        curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
#endif
    
    if (-1 != days)  clear_log(days);

    g_log_file = fopen(log_file_name, "ab+");//���λ��
    if (NULL == g_log_file)
    {
        return -2;
    }

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
#else

#endif

    return 0;
}

int close_log_file()
{
    if ((stderr != g_log_file) && (NULL != g_log_file))
    {
        fclose(g_log_file);
        g_log_file = NULL;
    }

    return 0;
}

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <tchar.h>

class CTraverseDir
{
public:
    CTraverseDir(void) {}
    ~CTraverseDir(void) {}

    //dir��β��Ҫ����б��
    int traverse_dir(const TCHAR* dir, bool recursion = true);

protected:
    //����0��ʾ���������� ����ֹͣ����
    virtual int file_operation(const TCHAR* file);
    //����0��ʾ���������� ����ֹͣ����
    virtual int dir_operation(const TCHAR* dir);
};

#endif

#if defined(__linux__) || defined(__linux)

class CTraverseDir
{
public:
    CTraverseDir(void) {}
    ~CTraverseDir(void) {}

    //dir��β��Ҫ����б��
    int traverse_dir(const char* dir, bool recursion = true);

protected:
    //����0��ʾ���������� ����ֹͣ����
    virtual int file_operation(const char* file);
    //����0��ʾ���������� ����ֹͣ����
    virtual int dir_operation(const char* dir);
};

#endif

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <atlstr.h>

int CTraverseDir::traverse_dir(const TCHAR* dir, bool recursion)
{
    WIN32_FIND_DATA find_file_data;
    HANDLE find_File = NULL;
    CString strDir = dir;
    strDir += _T("\\*.*");
    deque<CString> dir_array;
    dir_array.push_back(strDir);
    CString temp;

    while (!dir_array.empty())
    {
        find_File = FindFirstFile(dir_array.front(), &find_file_data);
        if (INVALID_HANDLE_VALUE == find_File)
        {
            return -1;
        }

        temp = dir_array.front().Left(dir_array.front().ReverseFind(_T('\\')) + 1);
        do
        {
            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (_tcscmp(find_file_data.cFileName, _T(".."))
                    && _tcscmp(find_file_data.cFileName, _T(".")))
                {
                    if (recursion)
                    {
                        strDir = temp + find_file_data.cFileName;
                        if (0 != dir_operation(strDir))
                        {
                            FindClose(find_File);
                            return -3;
                        }
                        strDir += _T("\\*.*");
                        dir_array.push_back(strDir);
                    }
                }
            }
            else
            {
                strDir = temp + find_file_data.cFileName;
                if (0 != file_operation(strDir))
                {
                    FindClose(find_File);
                    return -2;
                }
            }

        } while (FindNextFile(find_File, &find_file_data));

        dir_array.pop_front();
        FindClose(find_File);
    }

    return 0;
}


int CTraverseDir::file_operation(const TCHAR* file)
{
    return 0;
}


int CTraverseDir::dir_operation(const TCHAR* dir)
{
    return 0;
}
#endif


#if defined(__linux__) || defined(__linux)
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int CTraverseDir::traverse_dir(const char* dir, bool recursion)
{
    DIR* dp;
    struct dirent* entry;
    struct stat stat_buf;
    string strDir = dir;
    deque<string> dir_array;
    dir_array.push_back(strDir);

    while (!dir_array.empty())
    {
        if ((dp = opendir(dir_array.front().c_str())) == NULL)
        {
            return -1;
        }

        while ((entry = readdir(dp)) != NULL)
        {
            strDir = dir_array.front() + "/" + entry->d_name;
            lstat(strDir.c_str(), &stat_buf);
            if (S_ISDIR(stat_buf.st_mode))
            {
                if (strcmp(".", entry->d_name) && strcmp("..", entry->d_name))
                {
                    if (recursion)
                    {
                        dir_array.push_back(strDir);
                        if (0 != dir_operation(strDir.c_str()))
                        {
                            closedir(dp);
                            return -3;
                        }
                    }
                }
            }
            else
            {
                if (0 != file_operation(strDir.c_str()))
                {
                    closedir(dp);
                    return -2;
                }
            }
        }

        closedir(dp);
        dir_array.pop_front();
    }

    return 0;
}


int CTraverseDir::file_operation(const char* file)
{
    return 0;
}


int CTraverseDir::dir_operation(const char* dir)
{
    return 0;
}
#endif

class CCustomTraverseDir : public CTraverseDir
{
public:
    CCustomTraverseDir(const unsigned int days) : m_days(days) {}
private:
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    virtual int file_operation(const TCHAR* file);
#else
    virtual int file_operation(const char* file);
#endif
    unsigned int m_days;
};

static bool is_del_log(const char* date, const unsigned int days)
{
    struct tm begin = { 0 };
    int num = sscanf(date, "%04d-%02d-%02d-%02d-%02d-%02d", &begin.tm_year, &begin.tm_mon, &begin.tm_mday,
        &begin.tm_hour, &begin.tm_min, &begin.tm_sec);
    if (6 != num)  return false;
    begin.tm_year -= 1900;
    begin.tm_mon -= 1;
    time_t begin_second = mktime(&begin);
    if (-1 == begin_second)  return false;
    time_t end_second = 0;
    time(&end_second);
    if ((end_second - begin_second) / 3600 / 24 >= days) { return true; }
    return false;
}

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
int CCustomTraverseDir::file_operation(const TCHAR* file)
{
    const TCHAR* r =  _tcsrchr(file, L'\\');
    if (NULL == r)  { return 0; }
    const TCHAR* p = _tcschr(r, L'-');
    if (NULL == p)  { return 0; }
    const TCHAR* q = _tcschr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }

#if defined(_UNICODE) || defined(UNICODE)
    TCHAR file_name_key[PATHLEN] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, g_file_name_key, -1, file_name_key, PATHLEN);
    if ((p - r - 1) != _tcslen(file_name_key))  return 0;
    if (wmemcmp(r + 1, file_name_key, p - r - 1) != 0)  return 0;
#else
    if ((p - r - 1) != strlen(g_file_name_key))  return 0;
    if (memcmp(r + 1, g_file_name_key, p - r - 1) != 0)  return 0;
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
    if (is_del_log(date1, m_days))
#else
    if (is_del_log(date, m_days))
#endif
    {
        DeleteFile(file);
    }
    return 0;
}
#else
int CCustomTraverseDir::file_operation(const char* file)
{
    const char* r =  strrchr(file, L'/');
    if (NULL == r)  { return 0; }
    const char* p = strchr(r, L'-');
    if (NULL == p)  { return 0; }
    const char* q = strchr(file, L'.');
    if (NULL == q)  { return 0; }
    if (p >= q) { return 0; }

    if ((p - r - 1) != strlen(g_file_name_key))  return 0;
    if (memcmp(r + 1, g_file_name_key, p - r - 1) != 0)  return 0;

    char date[PATHLEN] = { 0 };
    int i = 0;
    p++;
    while (p != q)
    {
        date[i++] = *(p++);
    }
    date[i] = 0;

    if (is_del_log(date, m_days))
    {
        remove(file);
    }
    return 0;
}
#endif

int clear_log(const int days)
{
    CCustomTraverseDir traverse_dir(days);
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#if defined(_UNICODE) || defined(UNICODE)
    TCHAR log_dir[PATHLEN] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, g_log_dir, -1, log_dir, PATHLEN);
    return traverse_dir.traverse_dir(log_dir);
#else
    return traverse_dir.traverse_dir(g_log_dir);
#endif
#else
    return traverse_dir.traverse_dir(g_log_dir);
#endif
}

void set_log_level(int level)
{
    g_level = level;
}

int get_log_level()
{
    return g_level;
}

//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log1(int type, const char* file_name, int line, const char* func_name, const char *fmt, ...)
{
    if ((g_level & type) == 0)  return 0;
    char buf[4096];
    switch (type)
    {
    case LOG_ERROR_LEVEL:
    {
        buf[0] = 'E';
        break;
    }
    case LOG_NORMAL_LEVEL:
    {
        buf[0] = 'N';
        break;
    }
    case LOG_DEBUG_LEVEL:
    {
        buf[0] = 'D';
        break;
    }
    default:
        break;
    }

    buf[1] = '-';
    int offset = 2;
#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    int i = GetLastError();
    time_t curr_time;
    struct tm curr_tm;
    time(&curr_time);
    localtime_s(&curr_tm, &curr_time);
    LPVOID lpMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, i, 0, (LPSTR)&lpMsgBuf, 0, NULL);
    offset += sprintf(buf + offset, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s-%08x:\r\nSYS: %s",
        curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec,
        file_name, line, func_name, GetCurrentThreadId(), (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
    ///SetLastError(0);
#else
    char buf1[4096];
    char* p = strerror_r(errno, buf1, sizeof(buf1));//�̰߳�ȫ�汾��buf��һ����ֵ
    time_t curr_time;
    struct tm curr_tm;
    time(&curr_time);
    localtime_r(&curr_time, &curr_tm);
    offset += sprintf(buf + offset, "[%04d-%02d-%02d-%02d-%02d-%02d]%s-%d-%s-%08x:\nSYS: %s\n",
        curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec,
        file_name, line, func_name, (unsigned long)pthread_self(), p);
#endif

    va_list ap;
    va_start(ap, fmt);
    offset += vsprintf(buf + offset, fmt, ap);
    va_end(ap);

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
    offset += sprintf(buf + offset, "\r\n\r\n");
#else
    offset += sprintf(buf + offset, "\n\n");
#endif

    fwrite(buf, 1, offset, g_log_file);
    fflush(g_log_file);

    return 0;
}