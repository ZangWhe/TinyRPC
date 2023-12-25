#ifndef RPC_NET_WAKEUP_FD_EVENT_H
#define RPC_NET_WAKEUP_FD_EVENT_H

#include "RPC/net/fd_event.h"


namespace RPC{

    class WakeupFdEvent : public FdEvent{
        public:
            WakeupFdEvent(int fd);
            ~WakeupFdEvent();
            void wakeup();
        private:
            
    };
}

#endif