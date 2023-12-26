#ifndef RPC_NET_IO_THREAD_GROUP_H
#define RPC_NET_IO_THREAD_GROUP_H

#include<vector>
#include "RPC/net/io_thread.h"

namespace RPC{
    class IOThreadGroup{
        public:
            IOThreadGroup(size_t size);

            ~IOThreadGroup();

            void start();

            void join();

            IOThread* getIOThread();

        private:
            int m_size{0};      //  线程池大小

            std::vector<IOThread*> m_io_thread_groups;

            int m_index {0};    // 当前应该获取的IO线程的下标
    };
}

#endif