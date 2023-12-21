#ifndef RPC_NET_TIMEREVENT
#define RPC_NET_TIMEREVENT

#include <functional>
#include <memory>

namespace{
    class TimerEvent{
        public:
            typedef std::shared_ptr<TimerEvent> s_ptr;

            TimerEvent(int interval, bool is_repeated, std::function<void()> cb);
        
        private:
            int64_t m_arrive_time; //ms
            int64_t m_interval; //ms
            bool m_is_repeated{false};
            bool m_is_cancled{false};

            std::function<void()> m_task;
    }
}



#endif