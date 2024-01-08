#ifndef RPC_NET_TIMEREVENT
#define RPC_NET_TIMEREVENT

#include <functional>
#include <memory>

namespace RPC{
    class TimerEvent{
        public:
            typedef std::shared_ptr<TimerEvent> s_ptr;

            TimerEvent(int interval, bool is_repeated, std::function<void()> cb);
            int64_t getArriveTime() const{return m_arrive_time;}
            void setCancled(bool value){
                m_is_cancled = value;
            }
            bool isCancled(){
                return m_is_cancled;
            }
            bool isRepeated(){
                return m_is_repeated;
            }
            std::function<void()> getCallBack(){
                return m_task;
            }
            void resetArriveTime();
        private:
            int64_t m_arrive_time;          //ms, 执行任务的时间戳
            int64_t m_interval;             //ms, 执行任务的间隔
            bool m_is_repeated{false};
            bool m_is_cancled{false};

            std::function<void()> m_task;
    };
}



#endif