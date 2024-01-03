#include<sys/types.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/time.h>
#include<string.h>
#include<arpa/inet.h>

#include "util.h"

namespace RPC{
    static int32_t g_p_id = 0;
    static thread_local int32_t t_thread_id = 0;
    
    pid_t getPId(){
        if(g_p_id == 0){
            g_p_id = getpid();
        }
        
        return g_p_id;
    }

    pid_t getThreadId(){
        if(t_thread_id == 0){
            t_thread_id = syscall(SYS_gettid);
        }
        return t_thread_id;
    }
    
    // 返回当前毫秒数
    int64_t getNowMs(){
        timeval val;
        gettimeofday(&val,NULL);
        return val.tv_sec*1000 + val.tv_usec/1000;
    }

    int32_t getInt32FromNetByte(const char* buf){
        int32_t re;
        memcpy(&re,buf,sizeof(re));
        return ntohl(re);
    }
}
