
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include<string.h>

#include "/home/desktop/gitrep/TinyRPC/RPC/common/log.h"
#include "/home/desktop/gitrep/TinyRPC/RPC/common/util.h"
#include "/home/desktop/gitrep/TinyRPC/RPC/net/eventloop.h"


namespace RPC{

    #define ADD_TO_EPOLL()                                                                                                  \
    auto it = m_listen_fds.find(event->getFd());                                                                            \
    int op = EPOLL_CTL_ADD;                                                                                                 \
    if(it != m_listen_fds.end()){                                                                                           \
        op = EPOLL_CTL_MOD;                                                                                                 \
    }                                                                                                                       \
    epoll_event tmp_event = event->getEpollEvent();                                                                         \
    INFOLOG("epoll_event.events = %d", (int)tmp_event.events);                                                              \
    int rt = epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp_event);                                                            \
    if(rt == -1){                                                                                                           \
        ERRORLOG("faild epoll_ctl when add fd : %d , errno = %d, error info = %s",event->getFd(),errno,strerror(errno));    \
    }                                                                                                                       \
    m_listen_fds.insert(event->getFd());                                                                                    \
    DEBUGLOG("success to add event,fd[%d]",event->getFd());                                                                 \


    #define DELETE_FROM_EPOLL()                                                                                             \
    auto it = m_listen_fds.find(event->getFd());                                                                            \
                                                                                                                            \
    if(it == m_listen_fds.end()){                                                                                           \
        return;                                                                                                             \
    }                                                                                                                       \
    int op = EPOLL_CTL_DEL;                                                                                                 \
    epoll_event tmp_event = event->getEpollEvent();                                                                         \
    int rt = epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp_event);                                                            \
    if(rt == -1){                                                                                                           \
        ERRORLOG("faild epoll_ctl when add fd : %d , errno = %d, error info = %s",event->getFd(),errno,strerror(errno));    \
    }                                                                                                                       \
    DEBUGLOG("success to delete event,fd[%d]",event->getFd());                                                              \

    // 当前循环
    static thread_local EventLoop* t_current_eventloop = NULL;
    // 最大超时时间
    static int g_epoll_max_timeout = 10000;
    // 最大监听事件数
    static int g_epoll_max_events = 10;
    EventLoop::EventLoop(){
        if(t_current_eventloop != NULL){
            ERRORLOG("faild to create event loop,this thread has created eventloop");
            exit(0);
        }
        m_thread_id = getThreadId();
        
        m_epoll_fd = epoll_create(10);
        if(m_epoll_fd == -1){
            ERRORLOG("faild to create event loop,epoll_create error,error info [%d]",errno);
            exit(0);
        }
        INFOLOG("Start InitWakeFdEvent");
        initWakeupFdEvent();

        // epoll_event event;
        // event.events = EPOLLIN;
        // int rt = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_wakeup_fd, &event);
        // if(rt == -1){
        //     ERRORLOG("faild to create event loop,epoll_ctl eventfd error,error info [%d]",errno);
        //     exit(0);
        // }

        INFOLOG("success to create event loop in thread %d",m_thread_id);
        t_current_eventloop = this;
    }
    EventLoop::~EventLoop(){
        close(m_epoll_fd);
        if(m_wakeup_fd_event){
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = nullptr;
        }
    }
    void EventLoop::initWakeupFdEvent(){
        m_wakeup_fd = eventfd(0,EFD_NONBLOCK);
        if(m_wakeup_fd < 0){
            ERRORLOG("faild to create event loop,event_fd create error,error info [%d]",errno);
            exit(0);
        }
        INFOLOG("wakeup fd = %d", m_wakeup_fd);
        m_wakeup_fd_event = new WakeupFdEvent(m_wakeup_fd);
        m_wakeup_fd_event->listen(FdEvent::IN_EVENT,[this](){
            char buffer[8];
            DEBUGLOG("the buffer size is %d",sizeof(buffer));
            // while(read(m_fd,buffer,sizeof(buffer)) != -1 && errno == EAGAIN){
            while(read(m_wakeup_fd,buffer,8) != -1 && errno != EAGAIN){

            }
            DEBUGLOG("read full bytes from wakeup fd[%d]",m_wakeup_fd);
        });

        addEpollEvent(m_wakeup_fd_event);
    }
    void EventLoop::loop(){
        
        while(!m_stop_flag){
            
            // 加锁
            ScopeMutex<Mutex> lock(m_mutex);
            // 取出任务队列
            std::queue<std::function<void()>> tmp_tasks;
            m_pending_tasks.swap(tmp_tasks);
            lock.unlock();
            // 执行任务队列中的所有任务
            while(!tmp_tasks.empty()){
                std::function<void()> cb = tmp_tasks.front();
                tmp_tasks.pop();
                if(cb){
                    cb();
                }
            }
            
            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];

            DEBUGLOG("now begin to epoll_wait");

            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);

            DEBUGLOG("now end to epoll_wait,rt = [%d]",rt);

            if(rt < 0){
                ERRORLOG("epoll_wait error,error =[%d]",errno);
                // exit(0);
            }else{
                for(int i = 0; i < rt; i++){
                    
                    epoll_event trigger_event = result_events[i];
                    FdEvent* fd_event = static_cast<FdEvent*>(trigger_event.data.ptr);
                    if(fd_event == NULL){
                        ERRORLOG("fd_event = NULL, continue");
                        continue;
                    }
                    if(trigger_event.events & EPOLLIN){
                        DEBUGLOG("fd %d trigger EPOLLIN event",fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::IN_EVENT));
                    }
                    if(trigger_event.events & EPOLLOUT){
                        DEBUGLOG("fd %d trigger EPOLLOUT event",fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::OUT_EVENT));
                    }
                }
            }
        }
    }

    void EventLoop::wakeup(){
        m_wakeup_fd_event->wakeup();
    }
    void EventLoop::dealWakeup(){

    }
    void EventLoop::stop(){
        m_stop_flag = true;

    }

    void EventLoop::addEpollEvent(FdEvent* event){
        if(isInLoopThread()){
            ADD_TO_EPOLL();
        }else{
            auto cb =[this,event](){
                ADD_TO_EPOLL();
            };
            addTask(cb,true);
        }
    }

    void EventLoop::deleteEpollEvent(FdEvent* event){
        if(isInLoopThread()){
            DELETE_FROM_EPOLL();      
        }else{
            auto cb = [this,event](){
                DELETE_FROM_EPOLL();
            };
            addTask(cb,true);
        }
    }
    bool EventLoop::isInLoopThread(){
        return getThreadId() == m_thread_id;
    }

    void EventLoop::addTask(std::function<void()> cb,bool is_wake_up){
        ScopeMutex<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();

        if(is_wake_up){
            wakeup();
        }
    }
}