#ifndef ___LOG___
#define ___LOG___

//#define log1(userDescription)		
#define log1(userDescription)		_log1(__FILE__, __LINE__, __FUNCTION__, userDescription)


//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log(const char* fileName, int line, const char* funcName, const char *fmt, ...);

//�Զ���log����1  ����log������� + syserrno:ϵͳ��������,userDescription:�Զ�������
int _log1(const char* fileName, int line, const char* funcName, const char* userDescription = "");

#endif
