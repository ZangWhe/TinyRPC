#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "RPC/net/rpc/rpc_channel.h"
#include "RPC/net/rpc/rpc_controller.h"
#include "RPC/net/tcp/tcp_client.h"
#include "RPC/net/coder/tinypb_protocol.h"
#include "RPC/common/log.h"
#include "RPC/common/msg_id_util.h"
#include "RPC/common/error_code.h"

namespace RPC{
    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr){

    }

    RpcChannel::~RpcChannel(){

    }

    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done){
            
            std::shared_ptr<RPC::TinyPBProtocol> req_protocol = std::make_shared<RPC::TinyPBProtocol>();
            
            RpcController* m_controller = dynamic_cast<RpcController*>(controller);

            if(m_controller == NULL){
                ERRORLOG("faild callmethod, RpcController convert error");
                return ;
            }

            if(m_controller->GetMsgId().empty()){
                req_protocol->m_msg_id = MsgIDUtil::GenMsgID();
                m_controller->SetMsgId(req_protocol->m_msg_id);
            }else{
                req_protocol->m_msg_id = m_controller->GetMsgId();
            }

            req_protocol->m_method_name = method->full_name();
            INFOLOG("%s | call method name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());

            // serialize request
            if(!request->SerializeToString(&(req_protocol->m_pb_data))){
                std::string err_info = "faild to serialize";
                m_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
                ERRORLOG("%s | %s, origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
                return ;
            }


            // connect
            TcpClient client(m_peer_addr);
            
            client.connect([&client, req_protocol, done](){
                client.writeMessage(req_protocol, [&client, req_protocol, done](AbstractProtocol::s_ptr){
                    INFOLOG("%s | rpc send request success. method_name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());
                    client.readMessage(req_protocol->m_msg_id, [done](AbstractProtocol::s_ptr msg){
                        std::shared_ptr<RPC::TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<RPC::TinyPBProtocol>(msg);
                        INFOLOG("%s | get response success, call method name [%s]", rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str());

                        if(done){
                            done->Run();
                        }
                    });
                });
            });

    }
}