#include <pthread.h>
#include <assert.h>

#include "RPC/net/io_thread.h"
#include "RPC/common/log.h"
#include "RPC/common/util.h"

namespace RPC{
    IOThread::IOThread(){
        
        int rt = sem_init(&m_init_semaphore,0,0);
        assert(rt == 0);
        
        rt = sem_init(&m_start_semaphore,0,0);
        assert(rt == 0);

        pthread_create(&m_thread,NULL,&IOThread::Main,this);

        // 等待当前线程执行完Main函数的loop循环之前
        rt = sem_wait(&m_init_semaphore);
        
        DEBUGLOG("IOThread [%d] create success",m_thread_id);
    }

    IOThread::~IOThread(){
        m_event_loop->stop();
        sem_destroy(&m_init_semaphore);
        sem_destroy(&m_start_semaphore);

        pthread_join(m_thread, NULL);

        if(m_event_loop){
            delete m_event_loop;
            m_event_loop = NULL;
        }
    }

    EventLoop* IOThread::getEventLoop(){
        return m_event_loop;
    }

    void IOThread::start(){
        DEBUGLOG("Now invoke IOThread %d",m_thread_id);
        sem_post(&m_start_semaphore);
    }
    
    void IOThread::join(){
        pthread_join(m_thread,NULL);
    }

    void* IOThread::Main(void* arg){
        IOThread* thread = static_cast<IOThread*> (arg);

        thread->m_event_loop = new EventLoop();
        thread->m_thread_id = getThreadId();

        // 唤醒等待的线程
        sem_post(&thread->m_init_semaphore);
        DEBUGLOG("IOThread %d create. wait start semaphore", thread->m_thread_id);
        sem_wait(&thread->m_start_semaphore);
        DEBUGLOG("IOThread %d start loop",thread->m_thread_id);
        thread->m_event_loop->loop();
        DEBUGLOG("IOThread %d end loop",thread->m_thread_id);

        return NULL;
    }
} 