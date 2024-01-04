#ifndef RPC_NET_CODER_TINYPB_CODER_H
#define RPC_NET_CODER_TINYPB_CODER_H

#include "RPC/net/coder/abstract_coder.h"
#include "RPC/net/coder/tinypb_protocol.h"

namespace RPC{
    class TinyPBCoder : public AbstractCoder{
        public:
            TinyPBCoder();
            
            ~TinyPBCoder();
            void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer);

            void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer);
            
        private:
            std::pair<const char*,int> encodeTinyPB(std::shared_ptr<TinyPBProtocol> message);
            // const char* encodeTinyPB(std::shared_ptr<TinyPBProtocol> message, int& len);

    };
}


#endif