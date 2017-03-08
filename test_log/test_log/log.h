#ifndef LOG_H_
#define LOG_H_
#include <shlobj.h>
#include <stdio.h>

#define LOG_ERROR_LEVEL 1
#define LOG_NORMAL_LEVEL 2
#define LOG_DEBUG_LEVEL 4

#define LOG_ERROR(...)		if(get_log_level() & LOG_ERROR_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_NORMAL(...)		if(get_log_level() & LOG_NORMAL_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...)		if(get_log_level() & LOG_DEBUG_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

//fileNameKey��־�ļ����ƵĹؼ��֣���������-�����ַ��� ����������־���ܻ����
int open_log_file(const char* dir, const char* fileNameKey);
int close_log_file();

//�������ָ����������־��SetLogDir֮�����
int clear_log(const unsigned int days);
void set_log_level(int level = 1);
int get_log_level();

//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
