#include "RPC/common/log.h"
#include "RPC/net/tcp/net_addr.h"

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();
	
    RPC::IPNetAddr addr("127.0.0.1",12345);
    DEBUGLOG("create addr %s", addr.toString().c_str());


	return 0;
}