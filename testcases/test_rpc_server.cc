
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
#include "order.pb.h"

class OrderImpl : public Order{
    public:
        void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrderRequest* request,
                       ::makeOrderResponse* response,
                       ::google::protobuf::Closure* done){
                        APPDEBUGLOG("start sleep 5s");
                        sleep(5);
                        APPDEBUGLOG("end sleep 5s");
                        if(request->price() < 10){
                            response->set_ret_code(-1);
                            response->set_res_info("short balance");
                            return ;
                        }
                        response->set_order_id("20240106");
                        APPDEBUGLOG("call makeOrder success");
                       }
};

// extern RPC::Config* g_config;


int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Start test_rpc_server error, the number of parameter is wrong");
        printf("Please enter like this: \n");
        printf("./test_rpc_server ../conf/Tinyxml.xml\n");
        return 0;
    }
	RPC::Config::SetGlobalConfig(argv[1]);
	RPC::Logger::InitGlobalLogger();

    
	
    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    RPC::RpcDispatcher::GetRpcDispatcher()->registerService(service);

    RPC::IPNetAddr::s_ptr addr = std::make_shared<RPC::IPNetAddr>("127.0.0.1",RPC::Config::GetGlobalConfig()->m_port);

    DEBUGLOG("create addr %s", addr->toString().c_str());
	RPC::TcpServer tcp_server(addr);

	tcp_server.start();
	return 0;
}