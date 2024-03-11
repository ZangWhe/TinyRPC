#ifndef RPC_COMMON_UTIL_H
#define RPC_COMMON_UTIL_H

#include<sys/types.h>
#include<unistd.h>

namespace RPC{
    pid_t getPId();
    
    pid_t getThreadId();

    int64_t getNowMs();

    int32_t getInt32FromNetByte(const char* buf);

    double getCPUUtilization();     // 获取CPU利用率

    double getAverageLoad();        // 获取系统的平均负载能力
}

#endif