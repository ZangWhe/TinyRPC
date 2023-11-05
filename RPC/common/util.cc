#include<sys/types.h>
#include<unistd.h>
#include<sys/syscall.h>

#include "util.h"

namespace RPC{
    static int32_t g_p_id = 0;
    static thread_local int32_t g_thread_id = 0;
    pid_t getPId(){
        if(g_p_id == 0){
            g_p_id = getpid();
        }
        
        return g_p_id;
    }

    pid_t getThreadId(){
        if(g_thread_id == 0){
            g_thread_id = syscall(SYS_gettid);
        }
        return g_thread_id;
    }
}
