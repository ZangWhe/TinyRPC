#ifndef RPC_NET_STRING_CODER_H
#define RPC_NET_STRING_CODER_H

#include <string>

#include "RPC/net/abstract_coder.h"
#include "RPC/net/abstract_protocol.h"

namespace RPC{

    class StringProtocol : public AbstractProtocol{
        public:
            std::string info;
    };

    class StringCoder : public AbstractCoder{
        public:
            void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer){
                for(size_t i=0; i<messages.size(); i++){
                    std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
                    out_buffer->writeToBuffer(msg->info.c_str(), msg->info.length());
                }
            }

            void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer){
                
                std::vector<char> re;
                buffer->readFromBuffer(re, buffer->readAble());
                std::string info;
                for(size_t i=0;i<re.size();i++){
                    info += re[i];
                }

                std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
                msg->info = info;
                msg->setReqId("123456");
                out_messages.push_back(msg);
            }

    };
}

#endif