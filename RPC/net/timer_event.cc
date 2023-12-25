#include "RPC/net/timer_event.h"
#include "RPC/common/log.h"
#include "RPC/common/util.h"

namespace RPC{

    TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb) \
    : m_interval(interval), m_is_repeated(is_repeated), m_task(cb){
        resetArriveTime();
        
    }    
    void TimerEvent::resetArriveTime(){
        m_arrive_time = getNowMs() + m_interval;
        DEBUGLOG("success create timer event, will excute at [%lld]",m_arrive_time);
    }
}