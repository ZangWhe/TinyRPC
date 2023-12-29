#include "unistd.h"

#include "RPC/net/tcp/tcp_connection.h"
#include "RPC/net/fd_event_group.h"
#include "RPC/common/log.h"

namespace RPC{
    
    TcpConnection::TcpConnection(IOThread* io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr)
    : m_io_thread(io_thread), m_peer_addr(peer_addr), m_state(NotConnected), m_fd(fd){
        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();
        m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead,this));

        io_thread->getEventLoop()->addEpollEvent(m_fd_event);

    }

    TcpConnection::~TcpConnection(){
        DEBUGLOG("~TcpConnection");
    }

    void TcpConnection::onRead(){
        // 1. 从Socke缓冲区，调用系统的read函数读取字节到in_buffer里

        if(m_state != Connected){
            ERRORLOG("onRead error, client has already disconnected, addr [%s], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            return ;
        }
        bool is_read_all = false;
        bool is_close = false;
        while(!is_read_all){
            if(m_in_buffer->writeAble() == 0){
                m_in_buffer->resizeBuffer(2 * m_in_buffer->m_buffer.size());
            }
            int read_count = m_in_buffer->writeAble();
            int write_index = m_in_buffer->writeIndex();

            int rt = read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count);
            DEBUGLOG("success read %d bytes from addr [%s], clientfd [%d]", rt, m_peer_addr->toString().c_str(), m_fd);
            if(rt > 0){
                m_in_buffer->moveWriteIndex(rt);
                if(rt == read_count){
                    // 可能还没读完
                    continue;
                }else if(rt < read_count){
                    is_read_all = true;
                    break;
                }
            }else if(rt == 0){
                is_close = true;
                break;
            }else if(rt == -1 && errno == EAGAIN){
                is_read_all = true;
                break;
            }
        }
        if(is_close){
            DEBUGLOG("peer closed, peer addr [%s], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            clear();
            return ;
        }
        if(!is_read_all){
            ERRORLOG("not read all data");
        }
        // TODO: 简单echo ，后面补充RPC协议解析
        TcpConnection::excute();
    }

    void TcpConnection::excute(){
        // 将RPC请求执行业务逻辑，获取RPC相应，再把RPC响应发送回去
        std::vector<char> tmp_buffer;
        int size = m_in_buffer->readAble();
        tmp_buffer.resize(size);

        std::string msg  = "Hello Zhe Fuck";
        // for(int i=0; i<size; ++i){
        //     msg += tmp_buffer[i];
        // }

        m_in_buffer->readFromBuffer(tmp_buffer,size);
        INFOLOG("success get request [%s] from client [%s]",msg.c_str(), m_peer_addr->toString().c_str());
        
        m_out_buffer->writeToBuffer(msg.c_str(),msg.length());

        m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite,this));

        m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
    }

    void TcpConnection::onWrite(){
        // 将当前out_buffer 里的数据全部发送给client
        if(m_state != Connected){
            ERRORLOG("onWrite error, client has already disconnected, addr [%s], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            return ;
        }
        bool is_write_all = false;

        while(true){
            int write_size = m_out_buffer->readAble();
            if(write_size == 0){
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }

            int read_index = m_out_buffer->readIndex();
            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size);
            if(rt >= 0){
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }else if(rt == -1 && errno == EAGAIN){
                // 发送缓冲区已满，不能继续发送
                // 这种情况就等下次fd可写时再次发送数据即可
                ERRORLOG("write data error, rt == -1 && errno == EAGAIN");
                break;
            }
        }
        if(is_write_all){
            m_fd_event->cancle(FdEvent::OUT_EVENT);
            m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
        }
    }

    void TcpConnection::setState(const TcpState state){
        m_state = state;
    }

    TcpState TcpConnection::getState(){
        return m_state;
    }

    void TcpConnection::clear(){
        // 服务器处理关闭连接后的清理工作
        if(m_state == Closed){
            return ;
        }

        m_io_thread->getEventLoop()->deleteEpollEvent(m_fd_event);

        m_state = Closed;
    }

    void TcpConnection::shutdown(){
        if(m_state == Closed || m_state == NotConnected){
            return ;
        }
        // 处于半关闭
        m_state = HalfClosing;

        // 调用shutdown关闭读写，意味着服务器不在对这个fd进行读写操作
        // 发送FIN报文，触发四次挥手的第一个阶段
        // 当fd 发生可读事件，但是可读的数据为0,即对方也发送了一个FIN
        ::shutdown(m_fd,SHUT_RDWR);
    }
}