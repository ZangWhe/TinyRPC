#ifndef RPC_NET_FD_EVENT_GROUP_H
#define RPC_NET_FD_EVENT_GROUP_H

#include <vector>

#include "RPC/net/fd_event.h"
#include "RPC/common/mutex.h"

namespace RPC{
    class FdEventGroup{
        public:
            FdEventGroup(int size);

            ~FdEventGroup();

            FdEvent* getFdEvent(int fd);

        public:
            static FdEventGroup* GetFdEventGroup();

        private:
            int m_size {0};
            std::vector<FdEvent*> m_fd_group;
            Mutex m_mutex;
    };
    
    
}


#endif