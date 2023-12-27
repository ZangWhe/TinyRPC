#include<string.h>

#include "RPC/common/log.h"
#include "RPC/net/tcp/net_addr.h"

namespace RPC{

    IPNetAddr::IPNetAddr(const std::string& ip, uint16_t port) : m_ip(ip), m_port(port){
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        m_addr.sin_port = htons(m_port);
    }

    IPNetAddr::IPNetAddr(const std::string& addr){
        size_t idx = addr.find_first_of(":");
        if(idx == addr.npos){
            ERRORLOG("invalid ipve addr %s", addr.c_str());
            return ;
        }
        m_ip = addr.substr(0,idx);
        m_port - std::atoi(addr.substr(idx+1,addr.size() - idx - 1).c_str());

        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        m_addr.sin_port = htons(m_port);
    }

    IPNetAddr::IPNetAddr(sockaddr_in addr) : m_addr(addr){
        m_ip = std::string(inet_ntoa(m_addr.sin_addr));
        m_port = ntohs(m_addr.sin_port);

    }

    sockaddr* IPNetAddr::getSockAddr(){
        return reinterpret_cast<sockaddr*>(&m_addr);
    }

    socklen_t IPNetAddr::getSockLen(){
        return sizeof(m_addr);
    }

    int IPNetAddr::getFamily(){
        return AF_INET;
    }

    std::string IPNetAddr::toString(){
        std::string re;
        re = m_ip + ":" + std::to_string(m_port);
        return re;
    }

    bool IPNetAddr::checkValid(){
        if(m_ip.empty()){
            return false;
        }
        if(m_port < 0 || m_port > 65536){
            return false;
        }
        if(inet_addr(m_ip.c_str()) == INADDR_NONE){
            return false;
        }
        return true;
    }
}