#ifndef RPC_NET_TCP_TCP_SERVER_H
#define RPC_NET_TCP_TCP_SERVER_H

#include <set>

#include "RPC/net/tcp/tcp_acceptor.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/tcp/tcp_connection.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/io_thread_group.h"

namespace RPC{
    class TcpServer{
        public:
            TcpServer(NetAddr::s_ptr local_addr);

            ~TcpServer();

            void start();
            
        private:
            void init();

            // 当有新客户端连接时，执行的方法。
            void onAccept();

        private:
            TcpAcceptor::s_ptr m_acceptor;
            
            NetAddr::s_ptr m_local_addr;        // 本地监听地址

            EventLoop* m_main_event_loop {NULL};    // mainReactor

            IOThreadGroup* m_io_thread_group {NULL};    // subReactor

            FdEvent* m_listen_fd_event {NULL};

            int m_client_counts {0};

            std::set<TcpConnection::s_ptr> m_client;


    };
}

#endif