#include "/home/desktop/gitrep/TinyRPC/RPC/net/fd_event.h"
#include "/home/desktop/gitrep/TinyRPC/RPC/common/log.h"

namespace RPC
{
    FdEvent::FdEvent(int fd){

    }
    FdEvent::~FdEvent(){

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
         if(event_type == FdEvent::IN_EVENT){
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
         }else if(event_type ==FdEvent::OUT_EVENT){
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
         }
         m_listen_events.data.ptr = this;
    }
} // namespace RPC
