#include <memory>
#include <vector>
#include <string.h>

#include "RPC/net/coder/tinypb_coder.h"
#include "RPC/net/coder/tinypb_protocol.h"
#include "RPC/common/util.h"
#include "RPC/common/log.h"

namespace RPC{
    
    void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer){

    }

    void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer){
        // 遍历buffer：找到PB_START, 找到之后解析整包的长度，然后得到结束符的位置，判断是否是PB_END
        while(true){
            std::vector<char> tmp_buffer = buffer->m_buffer;
            int start_index = buffer->readIndex();
            int end_index = -1;

            int pack_len = 0;
            bool parse_success = false;
            int i = 0;
            for(i=start_index; i<buffer->writeIndex(); i++){
                if(tmp_buffer[i] == TinyPBProtocol::PB_START){
                    // 读取接下来的四个字节，，由于是网络字节序，需要转换为主机字节序
                    if(i+1 < buffer->writeIndex()){
                        pack_len = getInt32FromNetByte(&tmp_buffer[i+1]);
                        DEBUGLOG("get pack len = %d",ATOMIC_CHAR32_T_LOCK_FREE);

                        int j = i + pack_len - 1;
                        if(j >= buffer->writeIndex()){
                            continue;
                        }
                        if(tmp_buffer[j] == TinyPBProtocol::PB_END){
                            start_index = i;
                            end_index = j;
                            parse_success = true;
                            break;
                        }
                    }
                }
            }
            
            // 当前字节流读完
            if(i >= buffer->writeIndex()){
                DEBUGLOG("decode end, read all buffer data");
                return ;
            }


            if(parse_success){
                buffer->moveReadIndex(end_index - start_index + 1);

                std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
                
                // package length
                message->m_pack_len = pack_len;

                // message ID length
                int msg_id_len_index = start_index + sizeof(char) + sizeof(message->m_pack_len);
                if(msg_id_len_index >= end_index){
                    message->parse_success = false;
                    ERRORLOG("parse error, msg_id_len_index[%d] > end_index[%d]", msg_id_len_index, end_index);
                    continue;
                }
                message->m_msg_id_len = getInt32FromNetByte(&tmp_buffer[msg_id_len_index]);
                DEBUGLOG("parse msg_id_len=%d", message->m_msg_id_len);

                // message ID
                int msg_id_index = msg_id_len_index + sizeof(message->m_msg_id_len);
                char msg_id[128] = {0};
                memcpy(&msg_id[0], &tmp_buffer[msg_id_index], message->m_msg_id_len);
                message->m_msg_id = std::string(msg_id);
                DEBUGLOG("parse msg_id=%s", message->m_msg_id.c_str());

                // method name length
                int method_name_len_index = msg_id_index + message->m_msg_id_len;
                if(method_name_len_index >= end_index){
                    message->parse_success = false;
                    ERRORLOG("parse error, method_name_len_index[%d] > end_index[%d]", method_name_len_index, end_index);
                    continue;
                }
                message->m_method_name_len = getInt32FromNetByte(&tmp_buffer[method_name_len_index]);
                
                // method name
                int method_name_index = method_name_len_index + sizeof(message->m_method_name_len);
                char method_name[512] = {0};
                memcpy(&method_name[0], &tmp_buffer[method_name_index], message->m_method_name_len);
                message->m_method_name = std::string(method_name);
                DEBUGLOG("parse method_name=%s", message->m_method_name.c_str());

                // error code
                int err_code_index = method_name_index + message->m_method_name_len;
                if(err_code_index >= end_index){
                    message->parse_success = false;
                    ERRORLOG("parse error, err_code_index[%d] > end_index[%d]", err_code_index, end_index);
                    continue;
                }
                message->m_err_code = getInt32FromNetByte(&tmp_buffer[err_code_index]);

                // error info length
                int error_info_len_index = err_code_index + sizeof(message->m_err_code);
                if(error_info_len_index >= end_index){
                    message->parse_success = false;
                    ERRORLOG("parse error, error_info_len_index[%d] > end_index[%d]", error_info_len_index, end_index);
                    continue;
                }
                message->m_err_info_len = getInt32FromNetByte(&tmp_buffer[error_info_len_index]);
                
                // error info
                int error_info_index = error_info_len_index + sizeof(message->m_err_info_len);
                char error_info[512] = {0};
                memcpy(&error_info[0], &tmp_buffer[error_info_index], message->m_err_info_len);
                message->m_err_info = std::string(error_info);
                DEBUGLOG("parse error_info=%s", message->m_err_info.c_str());

                // protobuf seq length
                int pb_data_len = message->m_pack_len - message->m_method_name_len - message->m_msg_id_len - message->m_err_info_len - 2 - 24;
                // protobuf seq
                int pb_data_index = error_info_index + message->m_err_info_len;
                message->m_pb_data = std::string(&tmp_buffer[pb_data_index], pb_data_len);

                // 校验和解析
                message->parse_success = true;

                out_messages.push_back(message);
            }
        }
    }
}