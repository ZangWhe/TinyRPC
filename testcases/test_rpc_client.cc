
#include<stdio.h>
#include<assert.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>
#include<memory>
#include<unistd.h>
#include<google/protobuf/service.h>

#include "RPC/net/tcp/tcp_client.h"
#include "RPC/common/log.h"
#include "RPC/common/config.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/coder/string_coder.h"
#include "RPC/net/coder/abstract_protocol.h"
#include "RPC/net/coder/tinypb_coder.h"
#include "RPC/net/coder/tinypb_protocol.h"
#include "RPC/net/tcp/tcp_server.h"
#include "RPC/net/rpc/rpc_dispatcher.h"
#include "RPC/net/rpc/rpc_channel.h"
#include "RPC/net/rpc/rpc_controller.h"
#include "RPC/net/rpc/rpc_closure.h"
#include "order.pb.h"



void test_tcp_client(){
    RPC::IPNetAddr::s_ptr addr = std::make_shared<RPC::IPNetAddr>("127.0.0.1",12345);
    RPC::TcpClient client(addr);

    client.connect([addr, &client](){
        DEBUGLOG("connect to [%s] success",addr->toString().c_str());
        std::shared_ptr<RPC::TinyPBProtocol> message = std::make_shared<RPC::TinyPBProtocol>();
        message->m_msg_id = "99998888";
        message->m_pb_data = "test pb data wang !!!";

        makeOrderRequest request;
        request.set_price(100);
        request.set_goods("apple");
        if(!request.SerializeToString(&(message->m_pb_data))){
            ERRORLOG("serialize error");
            return ;
        }
        message->m_method_name = "Order.makeOrder";


        client.writeMessage(message, [request](RPC::AbstractProtocol::s_ptr msg_ptr){
            DEBUGLOG("send message success, request [%s]", request.ShortDebugString().c_str());
        });

        client.readMessage("99998888", [](RPC::AbstractProtocol::s_ptr msg_ptr){
            std::shared_ptr<RPC::TinyPBProtocol> message = std::dynamic_pointer_cast<RPC::TinyPBProtocol>(msg_ptr);
            DEBUGLOG("msg_id [%s] get response [%s] success", message->m_msg_id.c_str(),message->m_pb_data.c_str());
            makeOrderResponse response;
            if(!response.ParseFromString(message->m_pb_data)){
                ERRORLOG("deserilize error");
                return ;
            }
            DEBUGLOG("get response success, response [%s]",response.ShortDebugString().c_str());
        });

    });
}

void test_rpc_channel(){

    NEWRPCCHANNEL("127.0.0.1:12345",channel);

    NEWMESSAGE(makeOrderRequest, request);
    request->set_price(10086);
    request->set_goods("banana");
    
    NEWMESSAGE(makeOrderResponse, response);
    
    NEWRPCCONTROLLER(controller);
    controller->SetMsgId("99998888");
    controller->SetTimeout(10000);

    // 回调函数
    std::shared_ptr<RPC::RpcClosure> closure = std::make_shared<RPC::RpcClosure>([request,response,channel,controller]() mutable{
        if(controller->GetErrorCode() == 0){
            INFOLOG("call rpc success, request [%s], response [%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
        }else{
            ERRORLOG("call rpc error, request [%s], error code [%d], error info [%s]",
            request->ShortDebugString().c_str(),
            controller->GetErrorCode(),
            controller->GetErrorInfo().c_str());
        }

        channel->getTcpClient()->stop();
        channel.reset();
    });


    CALLRPC("127.0.0.1:12345", makeOrder, controller, request, response, closure);

}

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();

    // test_tcp_client();
    test_rpc_channel();

	return 0;
}