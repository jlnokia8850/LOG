#include "log.h"
#pragma warning(disable: 4996)



int main()
{
    open_log_file("D:\\log_test", "test");
    clear_log(1);  //删除1天前的日志

    char* file = "D:\\log_test\\123.txt";
    FILE* fp = fopen(file, "rb");
    if (NULL == fp)
    {
        log1("fopen %s error", file);
    }
    else
    {
        log1("fopen %s success", file);
    }

    log1("after open %s", file);
    close_log_file();
    return 0;
}

