#ifndef RPC_NET_EVENTLOOP_H
#define RPC_NET_EVENTLOOP_H


#include<pthread.h>
#include<set>
#include<functional>
#include<queue>

// #include "/home/desktop/gitrep/TinyRPC/RPC/common/mutex.h"
// #include "/home/desktop/gitrep/TinyRPC/RPC/net/fd_event.h"
// #include "/home/desktop/gitrep/TinyRPC/RPC/net/wakeup_fd_event.h"

#include "RPC/common/mutex.h"
#include "RPC/net/fd_event.h"
#include "RPC/net/wakeup_fd_event.h"
#include "RPC/net/timer.h"

namespace RPC{

    class EventLoop{

        public:
            EventLoop();
            ~EventLoop();

            void loop();

            void wakeup();

            void stop();

            void addEpollEvent(FdEvent* event);

            void deleteEpollEvent(FdEvent* event);

            bool isInLoopThread();

            void addTask(std::function<void()> cb,bool is_wake_up = false);

            void addTimerEvent(TimerEvent::s_ptr event);

        public:
            static EventLoop* getCurrentEventLoop();

        private:
            void dealWakeup();

            void initWakeupFdEvent();

            void initTimer();
            
        private:
            pid_t m_thread_id{0};

            int m_epoll_fd{0};

            int m_wakeup_fd{0};

            WakeupFdEvent* m_wakeup_fd_event {NULL};

            bool m_stop_flag{false};
            
            std::set<int> m_listen_fds;

            std::queue<std::function<void()>> m_pending_tasks;

            Mutex m_mutex;

            Timer* m_timer{NULL};

            
    };
}


#endif