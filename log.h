#ifndef ___LOG___
#define ___LOG___


#define log1(...)		_log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

//fileNameKey��־�ļ����ƵĹؼ��֣���������-�����ַ��� ����������־���ܻ����
int SetLogDir(const char* dir, const char* fileNameKey);

//�������ָ����������־��SetLogDir֮�����
int ClearLog(const unsigned int days);

//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
int _log1(const char* fileName, int line, const char* funcName, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
