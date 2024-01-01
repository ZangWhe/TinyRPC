#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include "RPC/net/tcp/tcp_client.h"
#include "RPC/net/eventloop.h"
#include "RPC/net/fd_event_group.h"
#include "RPC/common/log.h"

namespace RPC{
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr){
        m_event_loop = EventLoop::GetCurrentEventLoop();

        m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0);
        if(m_fd < 0){
            ERRORLOG("TcpClient Error, faild to create fd");
            return ;
        }
        
        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();

        m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, m_peer_addr, TcpConnectionByClient);

        m_connection->setConnectionType(TcpConnectionByClient);

    }

    TcpClient::~TcpClient(){
        if(m_fd > 0){
            close(m_fd);
        }
    }
    
    void TcpClient::connect(std::function<void()> done){
        int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
        if(rt == 0){
            DEBUGLOG("connect [%s] success", m_peer_addr->toString().c_str());
            if(done){
                done();
            }
        }else if(rt == -1){
            if(errno == EINPROGRESS){
                // epoll 监听可写事件，然后判断错误码
                m_fd_event->listen(FdEvent::OUT_EVENT,[this,done](){
                    int error = 0;
                    socklen_t error_len = sizeof(error);
                    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);
                    bool is_connect_success = false;
                    if(error == 0){
                        DEBUGLOG("connect [%s] success", m_peer_addr->toString().c_str());
                        is_connect_success = true;
                        m_connection->setState(TcpState::Connected); 
                    }else{
                        ERRORLOG("connect [%s] error, errno = %d, error = %s", m_peer_addr->toString().c_str(), errno, strerror(errno));
                    }
                    // 连接后需去掉可写事件的监听，不然会一直触发
                    m_fd_event->cancle(FdEvent::OUT_EVENT);
                    m_event_loop->addEpollEvent(m_fd_event);

                    // 如果连接成功，才会执行回调函数
                    if(is_connect_success && done){
                        done();
                        
                    }
                });
                m_event_loop->addEpollEvent(m_fd_event);
                if(!m_event_loop->isLooping()){
                    m_event_loop->loop();
                }
            }else{
                ERRORLOG("connect [%s] error, errno = %d, error = %s", m_peer_addr->toString().c_str(), errno, strerror(errno));
                return ;
            }
        }
    }

    void TcpClient::writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done){
        // 1. 把message对象写入到TcpConnection的buffer中,回调函数也写入
        // 2. 启动connection可写事件监听
        m_connection->pushSendMessage(message, done);
        m_connection->listenWrite();
    }

    void TcpClient::readMessage(const std::string& req_id, std::function<void(AbstractProtocol::s_ptr)> done){
        // 1. 监听可读事件
        // 2. 从buffer里decode得到message对象，判断与req_id是否相等，相等则读成功，执行回调函数
        m_connection->pushReadMessage(req_id, done);
        m_connection->listenRead();
    }
}