#include "log.h"
#include <stdio.h>
#pragma warning(disable: 4996)
//https://github.com/lihaitao8233

int main()
{
    open_log_file("D:\\log_test", "test");
    clear_log(1);  //删除1天前的日志
    set_log_level(LOG_ERROR_LEVEL | LOG_NORMAL_LEVEL);


    char* file = "D:\\log_test\\123.txt";
    FILE* fp = fopen(file, "rb");
    if (NULL == fp)
    {
        LOGE("fopen %s error", file);
    }
    else
    {
        LOGN("fopen %s success", file);
    }

    LOGD("after open %s", file);
    LOGN("before close_log_file");


    close_log_file();
    return 0;
}

