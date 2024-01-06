#ifndef RPC_NET_TCP_TCP_CONNECTION_H
#define RPC_NET_TCP_TCP_CONNECTION_H

#include <map>
#include <vector>
#include <memory>

#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/tcp/tcp_buffer.h"
#include "RPC/net/io_thread.h"
#include "RPC/net/fd_event.h"
#include "RPC/net/coder/abstract_protocol.h"
#include "RPC/net/coder/abstract_coder.h"
#include "RPC/net/rpc/rpc_dispatcher.h"

namespace RPC{
    enum TcpState{
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };

    enum TcpConnectionType{
        TcpConnectionByServer = 1,  // 作为服务端使用，代表与对端客户端的连接
        TcpConnectionByClient = 2,  // 作为客户端使用，代表与对端服务端的连接
    };
    class TcpConnection{
        public:
            typedef std::shared_ptr<TcpConnection> s_ptr;
        public:
            TcpConnection(EventLoop* event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, NetAddr::s_ptr local_addr, TcpConnectionType type = TcpConnectionByServer);

            ~TcpConnection();

            void onRead();

            void excute();

            void onWrite();

            void setState(const TcpState state);

            TcpState getState();

            void clear();

            void shutdown();    // 服务器主动关闭连接

            void setConnectionType(TcpConnectionType type);

            // 启动监听可写事件
            void listenWrite();

            // 启动监听可读事件
            void listenRead();

            void pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

            void pushReadMessage(const std::string& req_id, std::function<void(AbstractProtocol::s_ptr)> done);

            NetAddr::s_ptr getLocalAddr();

            NetAddr::s_ptr getPeerAddr();
        private:
            // IOThread* m_io_thread {nullptr};    // 持有该连接的IO线程
            EventLoop* m_event_loop {nullptr};
            
            NetAddr::s_ptr m_peer_addr {nullptr};
            NetAddr::s_ptr m_local_addr {nullptr};
            
            TcpBuffer::s_ptr m_in_buffer;   // 接收缓冲区
            TcpBuffer::s_ptr m_out_buffer;  // 发送缓冲区

            FdEvent* m_fd_event {nullptr};

            AbstractCoder* m_coder {nullptr};

            TcpState m_state; 
            
            int m_fd {0};

            TcpConnectionType m_connection_type {TcpConnectionByServer};

            // std::pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>>
            std::vector<std::pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>>> m_write_dones;

            std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_dones;

            // std::shared_ptr<RpcDispatcher> m_dispatcher;

    };
}


#endif