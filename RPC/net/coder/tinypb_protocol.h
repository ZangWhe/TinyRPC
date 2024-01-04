#ifndef RPC_NET_CODER_TINYPB_PROTOCOL_H
#define RPC_NET_CODER_TINYPB_PROTOCOL_H

#include <string>

#include "RPC/net/coder/abstract_coder.h"

namespace RPC{
    struct TinyPBProtocol : public AbstractProtocol{
        public:
            TinyPBProtocol();

            ~TinyPBProtocol();
        public:
            // 起始符和结束符
            static char PB_START;
            static char PB_END;

        public:
            int32_t m_pack_len {0};
            int32_t m_msg_id_len {0};
            // req_id继承父类
            int32_t m_method_name_len {0};
            std::string m_method_name;
            int32_t m_err_code {0};
            int32_t m_err_info_len {0};
            std::string m_err_info;
            std::string m_pb_data;
            int32_t m_check_sum{0};

            bool parse_success {false};
    };

    
}


#endif