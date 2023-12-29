#ifndef RPC_NET_TCP_TCP_CONNECTION_H
#define RPC_NET_TCP_TCP_CONNECTION_H

#include <memory>

#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/tcp/tcp_buffer.h"
#include "RPC/net/io_thread.h"
#include "RPC/net/fd_event.h"

namespace RPC{
    enum TcpState{
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };     
    class TcpConnection{
        public:
            typedef std::shared_ptr<TcpConnection> s_ptr;
        public:
            TcpConnection(IOThread* io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr);

            ~TcpConnection();

            void onRead();

            void excute();

            void onWrite();

            void setState(const TcpState state);

            TcpState getState();

            void clear();

            void shutdown();    // 服务器主动关闭连接
        private:
            IOThread* m_io_thread {nullptr};    // 持有该连接的IO线程

            NetAddr::s_ptr m_local_addr;
            NetAddr::s_ptr m_peer_addr;

            TcpBuffer::s_ptr m_in_buffer;   // 接收缓冲区
            TcpBuffer::s_ptr m_out_buffer;  // 发送缓冲区

            

            FdEvent* m_fd_event {nullptr};

            TcpState m_state; 
            
            int m_fd {0};

            

    };
}


#endif