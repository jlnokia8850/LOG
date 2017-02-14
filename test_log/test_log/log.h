#ifndef LOG_H_
#define LOG_H_
#include <shlobj.h>
#include <stdio.h>

#define log1(...)		_log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

//fileNameKey��־�ļ����ƵĹؼ��֣���������-�����ַ��� ����������־���ܻ����
int open_log_file(const char* dir, const char* fileNameKey);
int close_log_file();

//�������ָ����������־��SetLogDir֮�����
int clear_log(const unsigned int days);

//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
