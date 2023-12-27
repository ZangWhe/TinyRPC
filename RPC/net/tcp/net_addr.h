#ifndef RPC_NET_TCP_NET_ADDR_H
#define RPC_NET_TCP_NET_ADDR_H

#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>
#include<memory>

namespace RPC{
    class NetAddr{
        public:

            typedef std::shared_ptr<NetAddr> s_ptr;

            // 返回通用套接字结构
            virtual sockaddr* getSockAddr() = 0;

            // 
            virtual socklen_t getSockLen() = 0;

            // 返回套接字的协议族
            virtual int getFamily() = 0;

            // 地址转换字符串
            virtual std::string toString() = 0;

            // 检查IP地址合法性
            virtual bool checkValid() = 0;

      
    };

    class IPNetAddr : public NetAddr{
        public:

            IPNetAddr(const std::string& ip, uint16_t port);

            IPNetAddr(const std::string& addr);

            IPNetAddr(sockaddr_in addr);

            sockaddr* getSockAddr();

            socklen_t getSockLen();

            int getFamily();

            std::string toString();

            bool checkValid();

        private:
            std::string m_ip;
            uint16_t m_port {0};
            sockaddr_in m_addr;

    };
}

#endif