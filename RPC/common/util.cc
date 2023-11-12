#include<sys/types.h>
#include<unistd.h>
#include<sys/syscall.h>

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
}
