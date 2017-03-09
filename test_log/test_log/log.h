#ifndef LOG_H_
#define LOG_H_

#define LOG_ERROR_LEVEL     1
#define LOG_NORMAL_LEVEL    2
#define LOG_DEBUG_LEVEL     4
#define LOG_ERROR(...)		if(get_log_level() & LOG_ERROR_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_NORMAL(...)		if(get_log_level() & LOG_NORMAL_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...)		if(get_log_level() & LOG_DEBUG_LEVEL) _log1(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

//file_name_key��־�ļ����ƵĹؼ��֣���������-�����ַ��� ����������־���ܻ����
//��־�ļ�·���������£�ÿ�θ��ݳ�������ʱ�������µ���־�ļ���test��Ϊfile_name_key
//dir\test-2017-03-09-09-06-03.log
int open_log_file(const char* dir, const char* file_name_key);
int close_log_file();

//�������ָ����������־��open_log_file֮�����
int clear_log(const unsigned int days);
void set_log_level(int level = 1);
int get_log_level();

//����log����  ����� [��-��-��-ʱ-��-��]�ļ�-�к�-��������:
//ϵͳ��������
//�û�����
int _log1(const char* file_name, int line, const char* func_name, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
