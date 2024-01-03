#ifndef RPC_NET_ABSTRACT_CODER_H
#define RPC_NET_ABSTRACT_CODER_H

#include "RPC/net/tcp/tcp_buffer.h"
#include "RPC/net/coder/abstract_protocol.h"

namespace RPC{
    class AbstractCoder{
        public:
            // 将message对象转换为字节流，写入到buffer中
            virtual void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer) = 0;

            // 将buffer中的字节流转换为message对象
            virtual void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer) = 0;

            virtual ~AbstractCoder(){}
        // private:
        
    };
}

#endif