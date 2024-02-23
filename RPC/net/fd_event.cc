#include<string.h>
#include<fcntl.h>

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

    //设置非阻塞
void FdEvent::setNonBlock(){
        //获取文件描述符m_fd的当前状态
        int flag = fcntl(m_fd, F_GETFL, 0);
        //如果已经非阻塞，则直接返回
        if(flag & O_NONBLOCK){
            return ;
        }
        //设置文件描述符m_fd的状态为非阻塞
        fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    }

    std::function<void()> FdEvent::handler(TriggerEvent event_type){
        // 检查事件类型
        if(event_type == TriggerEvent::IN_EVENT){
            // 返回读取回调函数
            return m_read_callback;
        }else if(event_type == TriggerEvent::OUT_EVENT){
            // 返回写入回调函数
            return m_write_callback;
        }else if(event_type == TriggerEvent::ERROR_EVENT){
            // 返回错误回调函数
            return m_error_callback;
        }
        // 默认的处理或者什么也不做
        return []() {
            // 默认的处理或者什么也不做
         };
    }


    // 为FdEvent类添加listen方法，用于监听事件
    void FdEvent::listen(TriggerEvent event_type,std::function<void()> callback, std::function<void()> error_callback){
         // 如果事件类型为IN_EVENT，则监听EPOLLIN事件
         if(event_type == TriggerEvent::IN_EVENT){
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
         // 如果事件类型为OUT_EVENT，则监听EPOLLOUT事件
         }else if(event_type ==TriggerEvent::OUT_EVENT){
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
         }
         // 如果error_callback不为空，则将error_callback赋值给m_error_callback
         if(error_callback != nullptr){
            m_error_callback = error_callback;
         // 如果error_callback为空，则将m_error_callback赋值为nullptr
         }else{
            m_error_callback = nullptr;
         }
         // 将this赋值给m_listen_events.data.ptr
         m_listen_events.data.ptr = this;
    }

    void FdEvent::cancle(TriggerEvent event_type){
        if(event_type == TriggerEvent::IN_EVENT){
            m_listen_events.events &= (~EPOLLIN);
         }else{
            m_listen_events.events &= (~EPOLLOUT);
         }
    }
}
