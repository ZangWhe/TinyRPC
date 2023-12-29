#include<string.h>
#include<fcntl.h>

// #include "/home/desktop/gitrep/TinyRPC/RPC/net/fd_event.h"
// #include "/home/desktop/gitrep/TinyRPC/RPC/common/log.h"
#include "RPC/net/fd_event.h"
#include "RPC/common/log.h"

namespace RPC{
    FdEvent::FdEvent(int fd):m_fd(fd){
        memset(&m_listen_events,0,sizeof(m_listen_events));
        
    }
    FdEvent::FdEvent() {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::~FdEvent(){

    }

    void FdEvent::setNonBlock(){
        int flag = fcntl(m_fd, F_GETFL, 0);
        if(flag & O_NONBLOCK){
            return ;
        }
        fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    }

    std::function<void()> FdEvent::handler(TriggerEvent event_type){
        if(event_type == TriggerEvent::IN_EVENT){
            return m_read_callback;
        }else if(event_type == TriggerEvent::OUT_EVENT){
            return m_write_callback;
        }
        return []() {
            // 默认的处理或者什么也不做
         };
    }


    void FdEvent::listen(TriggerEvent event_type,std::function<void()> callback){
         if(event_type == TriggerEvent::IN_EVENT){
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
         }else{
            //  if(event_type ==TriggerEvent::OUT_EVENT)
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
         }
         m_listen_events.data.ptr = this;
    }

    void FdEvent::cancle(TriggerEvent event_type){
        if(event_type == TriggerEvent::IN_EVENT){
            m_listen_events.events &= (~EPOLLIN);
         }else{
            m_listen_events.events &= (~EPOLLOUT);
         }
    }
} // namespace RPC
