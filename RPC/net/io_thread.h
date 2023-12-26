#ifndef RPC_NET_IO_THREAD_H
#define RPC_NET_IO_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include "RPC/net/eventloop.h"

namespace RPC{
    class IOThread{
        public:
            static void* Main(void* arg);
        
        public:
            IOThread();

            ~IOThread();

            EventLoop* getEventLoop();

            void start();

            void join();
        
        private:
            pid_t m_thread_id {-1};          // 线程号
            pthread_t m_thread {0};         // 线程句柄
            EventLoop* m_event_loop{NULL};  // 当前IO线程的Loop对象

            sem_t m_init_semaphore;         // 初始信号量

            sem_t m_start_semaphore;        // 启动信号量
    };
}

#endif