#ifndef RPC_NET_CODER_TINYPB_CODER_H
#define RPC_NET_CODER_TINYPB_CODER_H

#include "RPC/net/coder/abstract_coder.h"

namespace RPC{
    class TinyPBCoder : public AbstractCoder{
        void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer);

        void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer);
    };
}


#endif