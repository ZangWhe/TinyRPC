#ifndef RPC_COMMON_UTIL_H
#define RPC_COMMON_UTIL_H

#include<sys/types.h>
#include<unistd.h>

namespace RPC{
    pid_t getPId();
    
    pid_t getThreadId();

    int64_t getNowMs();
}

#endif