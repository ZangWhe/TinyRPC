
#include "RPC/net/tcp/tcp_server.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/fd_event.h"
#include "RPC/net/io_thread_group.h"
#include "RPC/common/log.h"

namespace RPC{
    TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr){
        
        init();

        INFOLOG("RPC TcpSever listen success on [%s]", m_local_addr->toString().c_str());

    }

    TcpServer::~TcpServer(){
        if(m_main_event_loop){
            delete m_main_event_loop;
            m_main_event_loop = nullptr;
        }
        
    }

    void TcpServer::init(){
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

        m_main_event_loop = EventLoop::getCurrentEventLoop();

        m_io_thread_group = new IOThreadGroup(2);
        INFOLOG("the subReactor size is 2");

        m_listen_fd_event = new FdEvent(m_acceptor->getListenfd());

        m_listen_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpServer::onAccept,this));

        m_main_event_loop->addEpollEvent(m_listen_fd_event);
    }

    void TcpServer::onAccept(){
        int client_fd = m_acceptor->accept();
        // FdEvent client_fd_event(client_fd);
        m_client_counts++;

        // TODO: 把client_fd添加到任意IO线程中
        // m_io_thread_group->getIOThread()->getEventLoop()->addEpollEvent();

        INFOLOG("TcpServer success get client, fd = %d",client_fd);
    }
    
    void TcpServer::start(){
        m_io_thread_group->start();

        m_main_event_loop->loop();
    }
}