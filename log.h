#ifndef ___LOG___
#define ___LOG___


#define log1(...)		_log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

//fileNameKey日志文件名称的关键字，不允许有-减号字符， 否则清理日志功能会出错
int SetLogDir(const char* dir, const char* fileNameKey);

//清除超过指定天数的日志，SetLogDir之后调用
int ClearLog(const unsigned int days);

//基础log函数  输出： [年-月-日-时-分-秒]文件-行号-函数名称:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
