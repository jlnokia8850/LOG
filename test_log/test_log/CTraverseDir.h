#ifndef __TRAVERSEDIR__
#define __TRAVERSEDIR__
#include <stdio.h>

#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <tchar.h>

class CTraverseDir
{
public:

	CTraverseDir(void) {}
	~CTraverseDir(void) {}

	//dir结尾不要带有斜线
	int TraverseDir(const TCHAR* dir, bool isRecursion = true);
	int DelDir(const TCHAR* dir);


protected:

	//返回0表示继续遍历， 其它停止遍历
	virtual int FileOperation(const TCHAR* file);
	//返回0表示继续遍历， 其它停止遍历
	virtual int DirOperation(const TCHAR* dir);
};

#endif

#if defined(__linux__) || defined(__linux)

class CTraverseDir
{
public:

	CTraverseDir(void) {}
	~CTraverseDir(void) {}

	//dir结尾不要带有斜线
	int TraverseDir(const char* dir, bool isRecursion = true);
	int DelDir(const char* dir);


protected:

	//返回0表示继续遍历， 其它停止遍历
	virtual int FileOperation(const char* file);
	//返回0表示继续遍历， 其它停止遍历
	virtual int DirOperation(const char* dir);
};

#endif

#endif
