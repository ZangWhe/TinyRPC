#include<memory>
#include<string.h>

#include "RPC/common/log.h"
#include "RPC/net/tcp/tcp_buffer.h"

namespace RPC{
            TcpBuffer::TcpBuffer(int size):m_size(size){
                m_buffer.resize(size);
            }

            TcpBuffer::~TcpBuffer(){
                
            }

            // 返回可读字节数
            int TcpBuffer::readAble(){
                return m_write_index - m_read_index;
            }

            // 返回可写字节数
            int TcpBuffer::writeAble(){
                return m_buffer.size() - m_write_index;
            }

            int TcpBuffer::read_index(){
                return m_read_index;
            }

            int TcpBuffer::write_index(){
                return m_write_index;
            }

            void TcpBuffer::writeToBuffer(const char* buf, int size){
                if(size > TcpBuffer::writeAble()){
                    // 调整Buffer大小，扩容
                    int new_size = (int)(1.5 * (m_write_index + size));
                    resizeBuffer(new_size);
                }
                memcpy(&m_buffer[m_write_index], buf, size);
            }

            void TcpBuffer::readFromBuffer(std::vector<char>& readbuf, int size){
                int readAbleSize = TcpBuffer::readAble();
                if(readAbleSize == 0){
                    return ;
                }
                int read_size = readAbleSize > size ? size : readAbleSize;

                std::vector<char> tmp(read_size);
                memcpy(&tmp[0], &m_buffer[m_read_index], read_size);
                readbuf.swap(tmp);

                m_read_index += read_size;
                tmp.clear();

                TcpBuffer::adjustBuffer();
            }

            void TcpBuffer::resizeBuffer(int new_size){
                std::vector<char> tmp(new_size);
                int count = std::min(new_size,TcpBuffer::readAble());
                
                memcpy(&tmp[0], &m_buffer[m_read_index], count);
                m_buffer.swap(tmp);

                m_read_index = 0;
                m_write_index = m_read_index + count;

                tmp.clear();
            }

            void TcpBuffer::adjustBuffer(){
                int m_buffer_size = m_buffer.size();
                if(m_read_index < int(m_buffer_size / 3)){
                    return ;
                }
                std::vector<char> tmp_buffer(m_buffer_size);
                int count = TcpBuffer::readAble();

                memcpy(&tmp_buffer[0], &m_buffer[m_read_index], count);
                m_buffer.swap(tmp_buffer);
                
                m_read_index = 0;
                m_write_index = m_read_index + count;

                tmp_buffer.clear();
            }

            void TcpBuffer::moveReadIndex(int size){
                size_t target_index = m_read_index = size;
                if (target_index >= m_buffer.size()){
                    ERRORLOG("Error occured when move read index, invalid size %d, old read index %d, buffer size %d", size, m_read_index, m_buffer.size());
                    return ;
                }
                m_read_index = target_index;
                TcpBuffer::adjustBuffer();
            }

            void TcpBuffer::moveWriteIndex(int size){
                size_t target_index = m_write_index = size;
                if (target_index >= m_buffer.size()){
                    ERRORLOG("Error occured when move write index, invalid size %d, old wirte index %d, buffer size %d", size, m_write_index, m_buffer.size());
                    return ;
                }
                m_write_index = target_index;
            }
}