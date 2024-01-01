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


#include "RPC/net/tcp/tcp_client.h"
#include "RPC/common/log.h"
#include "RPC/common/config.h"
#include "RPC/net/tcp/net_addr.h"
#include "RPC/net/string_coder.h"
#include "RPC/net/abstract_protocol.h"

void test_connect(){
    // 调用connect 连接server
    // write一个字符串
    // 等待read返回结果
    int fd= socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        ERRORLOG("invalid fd %d", fd);
        exit(0);
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    int rt = connect(fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

    std::string msg = "Hello Fuck Wang";

    rt = write(fd, msg.c_str(), msg.length());
    DEBUGLOG("success write %d bytes, [%s]", rt, msg.c_str());

    char buf[100];
    rt = read(fd, buf, 100);
    
    DEBUGLOG("success read %d bytes, [%s]", rt, std::string(buf).c_str());
    
}

void test_tcp_client(){
    RPC::IPNetAddr::s_ptr addr = std::make_shared<RPC::IPNetAddr>("127.0.0.1",12345);
    RPC::TcpClient client(addr);

    client.connect([addr, &client](){
        DEBUGLOG("connect to [%s] success",addr->toString().c_str());
        std::shared_ptr<RPC::StringProtocol> message = std::make_shared<RPC::StringProtocol>();
        message->info = "Hello Fuck Wang";
        message->setReqId("123456");
        client.writeMessage(message, [](RPC::AbstractProtocol::s_ptr msg_ptr){
            DEBUGLOG("send message success");
        });

        client.readMessage("123456", [](RPC::AbstractProtocol::s_ptr msg_ptr){
            std::shared_ptr<RPC::StringProtocol> message_ptr = std::dynamic_pointer_cast<RPC::StringProtocol>(msg_ptr);
            DEBUGLOG("seq_id [%s] get response [%s] success", message_ptr->getReqId().c_str(),message_ptr->info.c_str());
        });

        client.writeMessage(message, [](RPC::AbstractProtocol::s_ptr msg_ptr){
            DEBUGLOG("send message 2222222 success");
        });
    });
}

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();
	
    // test_connect();
    test_tcp_client();
	return 0;
}