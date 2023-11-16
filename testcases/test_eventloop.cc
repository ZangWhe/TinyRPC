#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#include "../RPC/common/log.h"
#include "../RPC/common/config.h"
#include "../RPC/net/fd_event.h"
#include "../RPC/net/eventloop.h"


int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();
	
    RPC::EventLoop* eventloop = new RPC::EventLoop();
    
    
    int listen_fd = socket(AF_INET,SOCK_STREAM,0);

    if(listen_fd == -1){
        ERRORLOG("Socket Create Error");
        exit(0);
    }
    sockaddr_in addr;
    memset(&addr,0,sizeof(addr));

    addr.sin_port = htons(12345);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1",&addr.sin_addr);

    int rt = bind(listen_fd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
    if(rt != 0){
        ERRORLOG("Socket Bind Error");
        exit(0);
    }

    RPC::FdEvent event(listen_fd);
    event.listen(RPC::FdEvent::IN_EVENT,[listen_fd](){
        sockaddr_in peer_addr;
        socklen_t addr_len = 0;
        memset(&peer_addr,0,sizeof(peer_addr));
        int client_fd = accept(listen_fd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);

        inet_ntoa(peer_addr.sin_addr);
        DEBUGLOG("success get client fd[%d],peer addr:[%s:%d]",client_fd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));

    });

    eventloop->addEpollEvent(&event);

    eventloop->loop();
	return 0;
}
