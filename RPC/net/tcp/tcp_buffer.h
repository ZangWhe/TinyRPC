#ifndef RPC_NET_TCP_TCP_BUFFER_H
#define RPC_NET_TCP_TCP_BUFFER_H

#include<vector>
#include<iostream>



namespace RPC{
    class TcpBuffer{
        public:
            TcpBuffer(int size);

            ~TcpBuffer();

            // 返回可读字节数
            int readAble();

            // 返回可写字节数
            int writeAble();

            int read_index();

            int write_index();

            void writeToBuffer(const char* buf, int size);

            void readFromBuffer(std::vector<char>& readbuf, int size);

            void resizeBuffer(int new_size);

            void adjustBuffer();

            void moveReadIndex(int size);

            void moveWriteIndex(int size);

        

        private:
            int32_t m_read_index {0};
            int32_t m_write_index {0};
            int32_t m_size {0};
            
            std::vector<char> m_buffer;
    };
}


#endif