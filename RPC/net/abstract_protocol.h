#ifndef RPC_NET_ABSTRACT_PROTOCOL
#define RPC_NET_ABSTRACT_PROTOCOL

#include <memory>

namespace RPC{
    class AbstractProtocol{
        public:
            typedef std::shared_ptr<AbstractProtocol> s_ptr;
    };
}

#endif