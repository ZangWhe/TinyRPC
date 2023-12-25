#ifndef RPC_NET_TIMER_H
#define RPC_NET_TIMER_H

#include<map>
#include "RPC/net/fd_event.h"
#include "RPC/net/timer_event.h"
#include "RPC/common/mutex.h"

namespace RPC{

    class Timer : public FdEvent{
        public:
            Timer();
            ~Timer();
            void addTimerEvent(TimerEvent::s_ptr event);
            void deleteTimerEvent(TimerEvent::s_ptr event);
            void onTimer();     // 当发生IO事件后，eventlop会执行这回调函数
        private:
            void resetArriveTime();
        private:
            std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
            Mutex m_mutex;

    };
}

#endif