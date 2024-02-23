#ifndef RPC_COMMON_RUN_TIME_H
#define RPC_COMMON_RUN_TIME_H

#include <string>
#include "RPC/net/rpc/rpc_interface.h"

namespace RPC{
    class RunTime{
        public:

        public:
            static RunTime* GetRunTime();

            // static void SetRunTime();

        public:
            std::string m_msg_id;

            std::string m_method_name;

            RpcInterface* m_rpc_interface {NULL};
    };
}

#endif