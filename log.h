/*
���ߣ����
QQ:   243660565
*/
#ifndef ___LOG___
#define ___LOG___


//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int log(char* fileName, int line, char* funcName, char *fmt, ...);

//�Զ���log����1  ����log������� + syserrno:ϵͳ��������,userDescription:�Զ�������
int log1(char* fileName, int line, char* funcName, char* userDescription = "");

#endif
