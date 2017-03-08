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

//fileNameKey日志文件名称的关键字，不允许有-减号字符， 否则清理日志功能会出错
int open_log_file(const char* dir, const char* fileNameKey);
int close_log_file();

//清除超过指定天数的日志，SetLogDir之后调用
int clear_log(const unsigned int days);
void set_log_level(int level = 1);
int get_log_level();

//基础log函数  输出： [年-月-日-时-分-秒]文件-行号-函数名称:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
