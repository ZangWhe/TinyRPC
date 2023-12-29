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

#include "RPC/common/log.h"
#include "RPC/common/config.h"

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

int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();
	
    test_connect();

	return 0;
}