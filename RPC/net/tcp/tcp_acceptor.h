#ifndef RPC_NET_TCP_TCP_ACCEPOR_H
#define RPC_NET_TCP_TCP_ACCEPOR_H

#include "RPC/net/tcp/net_addr.h"

namespace RPC{
    class TcpAcceptor{
      public:
        TcpAcceptor(NetAddr::s_ptr local_addr);

        ~TcpAcceptor();

        int accept();

      private:
        // 发服务端监听的地址, addr -> ip:port
        NetAddr::s_ptr m_local_addr;
        // 协议族
        int m_family {-1};
        // 监听套接字
        int m_listenfd {-1};

    };
}

#endif