/*
���ߣ����
QQ:   243660565
*/
#ifndef ___LOG___
#define ___LOG___

//#define log1(userDescription)		
#define log1(userDescription)		_log1(__FILE__, __LINE__, __FUNCTION__, userDescription)


//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log(char* fileName, int line, char* funcName, char *fmt, ...);

//�Զ���log����1  ����log������� + syserrno:ϵͳ��������,userDescription:�Զ�������
int _log1(char* fileName, int line, char* funcName, char* userDescription = "");

#endif
