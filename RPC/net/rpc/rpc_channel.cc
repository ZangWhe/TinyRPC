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
        INFOLOG("~RpcChannel");
    }

    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done){
            
            // Init(controller,request,response,done);

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

            if(!m_is_init){
                std::string err_info = "RPC Channel not init";
                m_controller->SetError(ERROR_RPC_CHANNEL_INIT, err_info);
                ERRORLOG("%s | %s, RPC Channel not init", req_protocol->m_msg_id.c_str(), err_info.c_str());
                return ;
            }

            // serialize request
            if(!request->SerializeToString(&(req_protocol->m_pb_data))){
                std::string err_info = "faild to serialize";
                m_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
                ERRORLOG("%s | %s, origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
                return ;
            }

            s_ptr channel = shared_from_this();

            // connect
            TcpClient client(m_peer_addr);
            
            client.connect([&client, req_protocol, channel]() mutable{
                client.writeMessage(req_protocol, [&client, req_protocol, channel](AbstractProtocol::s_ptr) mutable{
                    INFOLOG("%s | rpc send request success. method_name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());
                    client.readMessage(req_protocol->m_msg_id, [channel](AbstractProtocol::s_ptr msg) mutable{
                        std::shared_ptr<RPC::TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<RPC::TinyPBProtocol>(msg);
                        INFOLOG("%s | get response success, call method name [%s]", rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str());

                        RpcController* m_controller = dynamic_cast<RpcController*>(channel->getController());

                        if(!(channel->getResponse()->ParseFromString(rsp_protocol->m_pb_data))){
                            ERRORLOG("%s | serilize error", rsp_protocol->m_msg_id.c_str());
                            m_controller->SetError(ERROR_FAILED_SERIALIZE, "serialize error");
                            return ;
                        }
                        if(rsp_protocol->m_err_code != 0){
                            ERRORLOG("%s | call rpc method [%s] faild, error code [%d], error info [%s]",
                                rsp_protocol->m_msg_id.c_str(), 
                                rsp_protocol->m_method_name.c_str(),
                                rsp_protocol->m_err_code,
                                rsp_protocol->m_err_info.c_str());
                            m_controller->SetError(rsp_protocol->m_err_code, rsp_protocol->m_err_info);
                            return ;
                        }
                        if(channel->getClosure()){
                            channel->getClosure()->Run();
                        }
                        
                        channel.reset();
                    });
                });
            });

    }


    void RpcChannel::Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr res, closure_s_ptr done){
        if(m_is_init){
            return ;
        }
        m_controller = controller;
        m_request = req;
        m_response = res;
        m_closure = done;
        m_is_init = true;
    }

    google::protobuf::RpcController* RpcChannel::getController(){
        return m_controller.get();
    }

    google::protobuf::Message* RpcChannel::getRequest(){
        return m_request.get();
    }

    google::protobuf::Message* RpcChannel::getResponse(){
        return m_response.get();
    }

    google::protobuf::Closure* RpcChannel::getClosure(){
        return m_closure.get();
    }

}