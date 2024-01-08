#ifndef RPC_NET_TCP_TCP_CLIENT
#define RPC_NET_TCP_TCP_CLIENT

#include <memory>

#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/tcp/tcp_connection.h"
#include "RPC/net/coder/abstract_protocol.h"
#include "RPC/net/timer_event.h"

namespace RPC{
    class TcpClient{
        public:
            typedef std::shared_ptr<TcpClient> s_ptr;

            TcpClient(NetAddr::s_ptr peer_addr);

            ~TcpClient();

            // 异步连接connect
            // 如果连接成功，执行done回调函数
            void connect(std::function<void()> done);     

            // 异步发送msg
            // 如果发送成功，会调用回调函数done，函数的入参就是msg对象
            void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);            

            // 异步读取msg
            // 如果读取成功，会调用回调函数done，函数的入参就是msg对象
            void readMessage(const std::string& req_id, std::function<void(AbstractProtocol::s_ptr)> done);

            void stop();

            int getConnectErrorCode();

            std::string getConnectErrorInfo();

            NetAddr::s_ptr getPeerAddr();

            NetAddr::s_ptr getLocalAddr();

            void initLocalAddr();

            void addTimerEvent(TimerEvent::s_ptr timer_event);

        private:
            NetAddr::s_ptr m_peer_addr {nullptr};

            NetAddr::s_ptr m_local_addr {nullptr};

            EventLoop* m_event_loop {nullptr};

            int m_fd {-1};

            FdEvent* m_fd_event {nullptr};

            TcpConnection::s_ptr m_connection {nullptr};

            int m_connect_error_code {0};

            std::string m_connect_error_info;
    };
}
#endif