#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<memory>

#include "RPC/common/log.h"
#include "RPC/common/config.h"
#include "RPC/net/fd_event.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/timer_event.h"
#include "RPC/net/io_thread.h"
#include "RPC/net/io_thread_group.h"

void test_io_thread(){

    int listen_fd = socket(AF_INET,SOCK_STREAM,0);
    
    if(listen_fd == -1){
        ERRORLOG("Socket Create Error");
        exit(1);
    }
    sockaddr_in addr;
    memset(&addr,0,sizeof(addr));

    addr.sin_port = htons(12345);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1",&addr.sin_addr);

    int rt = bind(listen_fd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
    if(rt != 0){
        ERRORLOG("Socket Bind Error");
        exit(1);
    }
    rt = listen(listen_fd,100);
    if(rt != 0){
        ERRORLOG("Socket Listen Error");
        exit(1);
    }
    RPC::FdEvent event(listen_fd);
    event.listen(RPC::FdEvent::IN_EVENT,[listen_fd](){
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr,0,sizeof(peer_addr));
        int client_fd = accept(listen_fd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);

        inet_ntoa(peer_addr.sin_addr);
        DEBUGLOG("success get client fd[%d],peer addr:[%s:%d]",client_fd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));

    });

  
    int i = 0;
    RPC::TimerEvent::s_ptr timer_event = std::make_shared<RPC::TimerEvent>(
        1000, true, [&i](){
            INFOLOG("trigger timer event,count = %d",i++);
        }
    );

    RPC::IOThreadGroup io_thread_group(2);

    RPC::IOThread* io_thread = io_thread_group.getIOThread();
    io_thread->getEventLoop()->addEpollEvent(&event);
    io_thread->getEventLoop()->addTimerEvent(timer_event);

    RPC::IOThread* io_thread2 = io_thread_group.getIOThread();
    io_thread2->getEventLoop()->addTimerEvent(timer_event);

    io_thread_group.start();
    io_thread_group.join();

}

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();

    test_io_thread();
    
	return 0;
}
