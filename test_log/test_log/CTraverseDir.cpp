#include "stdafx.h"
#include "CTraverseDir.h"
#include <string>
#include <vector>
#include <deque>
using namespace std;


#if defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE)
#include <atlstr.h>

int CTraverseDir::TraverseDir(const TCHAR* dir, bool isRecursion)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = NULL;
	CString strDir = dir;
	strDir +=  _T("\\*.*");
	deque<CString> strDirArray;
	strDirArray.push_back(strDir);
	CString strTemp;

	while (!strDirArray.empty())
	{
		hFindFile = FindFirstFile(strDirArray.front(), &FindFileData);
		if (INVALID_HANDLE_VALUE == hFindFile)
		{
			return -1;
		}

		strTemp = strDirArray.front().Left(strDirArray.front().ReverseFind(_T('\\')) + 1);
		do 
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FindFileData.cFileName, _T(".."))
					&& _tcscmp(FindFileData.cFileName, _T(".")))
				{
					if (isRecursion)
					{
						///strDir = strDirArray.front();
						///strDir = strDir.Left(strDir.ReverseFind(_T('\\')) + 1);
						strDir = strTemp + FindFileData.cFileName;
						if (0 != DirOperation(strDir))
						{
							FindClose(hFindFile);
							return -3;
						}	
						strDir += _T("\\*.*");
						strDirArray.push_back(strDir);
					}
				}
			}
			else
			{	
				///strDir = strDirArray.front();
				///strDir = strDir.Left(strDir.ReverseFind(_T('\\')) + 1);
				strDir = strTemp + FindFileData.cFileName;
				if (0 != FileOperation(strDir))
				{
					FindClose(hFindFile);
					return -2;
				}	
			}

		} while (FindNextFile(hFindFile, &FindFileData));


		strDirArray.pop_front();
		FindClose(hFindFile);
	}

	return 0;
}


int CTraverseDir::FileOperation(const TCHAR* file)
{
	return 0;
}


int CTraverseDir::DirOperation(const TCHAR* dir)
{
	return 0;
}


int CTraverseDir::DelDir(const TCHAR* dir)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = NULL;
	CString strDir = dir;
	strDir +=  _T("\\*.*");
	vector<CString> strDirArray;
	strDirArray.push_back(strDir);
	vector<CString>::iterator iter = strDirArray.begin();
	int count = 0;

	while (iter != strDirArray.end())
	{
		hFindFile = FindFirstFile(*iter, &FindFileData);
		if (INVALID_HANDLE_VALUE == hFindFile)
		{
			return -1;
		}

		do 
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FindFileData.cFileName, _T(".."))
					&& _tcscmp(FindFileData.cFileName, _T(".")))
				{
					iter = strDirArray.begin() + count;
					strDir = *iter;
					strDir = strDir.Left(strDir.ReverseFind(_T('\\')) + 1);
					strDir += FindFileData.cFileName;
					strDir += _T("\\*.*");
					strDirArray.push_back(strDir);
				}
			}
			else
			{
				iter = strDirArray.begin() + count;
				strDir = *iter;
				strDir = strDir.Left(strDir.ReverseFind(_T('\\')) + 1);
				strDir += FindFileData.cFileName;
				SetFileAttributes(strDir, FILE_ATTRIBUTE_NORMAL);
				DeleteFile(strDir);
			}

		} while (FindNextFile(hFindFile, &FindFileData));

		count++;
		iter = strDirArray.begin() + count;
		FindClose(hFindFile);
	}

	vector<CString>::reverse_iterator iter1;
	for (iter1 = strDirArray.rbegin(); iter1 != strDirArray.rend(); iter1++)
	{
		strDir = *iter1;
		strDir = strDir.Left(strDir.ReverseFind(_T('\\')));
		RemoveDirectory(strDir);
	}

	return 0;
}
#endif



#if defined(__linux__) || defined(__linux)
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int CTraverseDir::TraverseDir(const char* dir, bool isRecursion)
{
	DIR* dp;
	struct dirent* entry;
	struct stat statbuf;
	string strDir = dir;
	deque<string> strDirArray;
	strDirArray.push_back(strDir);

	while (!strDirArray.empty())
	{
		if ((dp = opendir(strDirArray.front().c_str())) == NULL)
		{
			return -1;
		}

		while ((entry = readdir(dp)) != NULL)
		{
			strDir = strDirArray.front() + "/" + entry->d_name;
			lstat(strDir.c_str(), &statbuf);
			if (S_ISDIR(statbuf.st_mode))
			{
				if (strcmp(".", entry->d_name) && strcmp("..", entry->d_name))
				{
					if (isRecursion)
					{
						strDirArray.push_back(strDir);
						if (0 != DirOperation(strDir.c_str()))
						{
							closedir(dp);
							return -3;
						}	
					}
				}
			}
			else
			{
				if (0 != FileOperation(strDir.c_str()))
				{
					closedir(dp);
					return -2;
				}
			}
		}

		closedir(dp);
		strDirArray.pop_front();
	}

	return 0;
}


int CTraverseDir::FileOperation(const char* file)
{
	return 0;
}


int CTraverseDir::DirOperation(const char* dir)
{
	return 0;
}
#endif