#include <unistd.h>

#include "/home/desktop/gitrep/TinyRPC/RPC/net/wakeup_fd_event.h"
#include "/home/desktop/gitrep/TinyRPC/RPC/common/log.h"

namespace RPC{
    WakeupFdEvent::WakeupFdEvent(int fd):FdEvent(fd){
        
    }

    WakeupFdEvent::~WakeupFdEvent(){

    }
    
    void WakeupFdEvent::wakeup(){
        char buffer[8] = {'a'};
        int rt = write(m_fd,buffer,8);
        if(rt != 8){
            ERRORLOG("write to wakeup fd less than 8 bytes,fd[%d]",m_fd);   
        }
        DEBUGLOG("success read 8 bytes");
    }
}