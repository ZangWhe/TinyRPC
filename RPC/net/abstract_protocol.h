#ifndef RPC_NET_ABSTRACT_PROTOCOL
#define RPC_NET_ABSTRACT_PROTOCOL

#include <memory>

namespace RPC{
    // : public std::enable_shared_from_this<AbstractProtocol>
    class AbstractProtocol{
        public:
            typedef std::shared_ptr<AbstractProtocol> s_ptr;

            virtual ~AbstractProtocol(){}

            std::string getReqId(){
                return m_req_id;
            }
            void setReqId(const std::string& req_id){
                m_req_id = req_id;
            }
            
        protected:
            std::string m_req_id;   // 请求号，唯一标识一个请求或者响应

    };
}

#endif