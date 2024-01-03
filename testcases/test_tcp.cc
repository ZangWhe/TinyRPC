#include <memory>

#include "RPC/common/log.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/tcp/tcp_server.h"
void test_tcp_server(){
	RPC::IPNetAddr::s_ptr addr = std::make_shared<RPC::IPNetAddr>("127.0.0.1",12345);
    DEBUGLOG("create addr %s", addr->toString().c_str());
	RPC::TcpServer tcp_server(addr);

	tcp_server.start();

}

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();

    test_tcp_server();
	return 0;
}