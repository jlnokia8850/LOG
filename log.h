/*
作者：李海涛
QQ:   243660565
*/
#ifndef ___LOG___
#define ___LOG___

//#define log1(userDescription)		
#define log1(userDescription)		_log1(__FILE__, __LINE__, __FUNCTION__, userDescription)


//基础log函数  输出： [年-月-日-时-分-秒]文件-行号-函数名称:
int _log(char* fileName, int line, char* funcName, char *fmt, ...);

//自定义log函数1  基础log函数输出 + syserrno:系统错误描述,userDescription:自定义描述
int _log1(char* fileName, int line, char* funcName, char* userDescription = "");

#endif
