
#include "RPC/net/tcp/tcp_server.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/fd_event.h"
#include "RPC/net/io_thread_group.h"
#include "RPC/net/tcp/tcp_connection.h"
#include "RPC/common/log.h"
#include "RPC/common/config.h"

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

        m_main_event_loop = EventLoop::GetCurrentEventLoop();

        m_io_thread_group = new IOThreadGroup(Config::GetGlobalConfig()->m_io_threads);

        m_listen_fd_event = new FdEvent(m_acceptor->getListenfd());

        m_listen_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpServer::onAccept,this));

        m_main_event_loop->addEpollEvent(m_listen_fd_event);
    }

    void TcpServer::onAccept(){
        auto re = m_acceptor->accept();
        int client_fd = re.first;
        NetAddr::s_ptr peer_addr = re.second;
        
        m_client_counts++;

        // 把client_fd添加到任意IO线程中
        IOThread* io_thread = m_io_thread_group->getIOThread();
        TcpConnection::s_ptr connection = std::make_shared<TcpConnection>(io_thread->getEventLoop(), client_fd, 128, peer_addr, m_local_addr);
        connection->setState(Connected);
        m_client.insert(connection);
        // 需要删除掉关闭的连接
        INFOLOG("TcpServer success get client, fd = %d",client_fd);
    }
    
    void TcpServer::start(){
        m_io_thread_group->start();

        m_main_event_loop->loop();
    }
}