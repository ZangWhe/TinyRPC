#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <unistd.h>
#include <google/protobuf/service.h>
#include <etcd/Client.hpp>

#include "RPC/common/log.h"
#include "RPC/common/config.h"
#include "RPC/common/log.h"
#include "RPC/net/tcp/tcp_client.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/coder/string_coder.h"
#include "RPC/net/coder/abstract_protocol.h"
#include "RPC/net/coder/tinypb_coder.h"
#include "RPC/net/coder/tinypb_protocol.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/tcp/tcp_server.h"
#include "RPC/net/rpc/rpc_dispatcher.h"

#include "order.pb.h"

class OrderImpl : public Order
{
public:
    void makeOrder(google::protobuf::RpcController *controller,
                   const ::makeOrderRequest *request,
                   ::makeOrderResponse *response,
                   ::google::protobuf::Closure *done)
    {
        APPDEBUGLOG("start sleep 5s");
        sleep(5);
        APPDEBUGLOG("end sleep 5s");
        if (request->price() < 10)
        {
            response->set_ret_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20230514");
        APPDEBUGLOG("call makeOrder success");
        if (done)
        {
            done->Run();
            delete done;
            done = NULL;
        }
    }
};

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Start test_rpc_server error, argc not 2 \n");
        printf("Start like this: \n");
        printf("./test_rpc_server ../conf/RPC.xml \n");
        return 0;
    }

    RPC::Config::SetGlobalConfig(argv[1]);

    RPC::Logger::InitGlobalLogger(0);

    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    // rpc注册
    RPC::RpcDispatcher::GetRpcDispatcher()->registerService(service);

    // etcd 服务注册
    etcd::Client etcd("http://127.0.0.1:2379");
    
    std::string value_addr = "127.0.0.1:" + std::to_string(RPC::Config::GetGlobalConfig()->m_port);
    std::string key_service = "makeOrder";
    etcd::Response response = etcd.ls(key_service).get();
    key_service += std::to_string(response.keys().size());
    
    etcd.set(key_service, value_addr).wait();
    INFOLOG("etcd registed success, Key : %s  Value : %s", key_service.c_str(), value_addr.c_str());

    RPC::IPNetAddr::s_ptr addr = std::make_shared<RPC::IPNetAddr>("127.0.0.1", RPC::Config::GetGlobalConfig()->m_port);

    RPC::TcpServer tcp_server(addr);

    tcp_server.start();

    return 0;
}