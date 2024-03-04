#ifndef RPC_COMMON_RUN_TIME_H
#define RPC_COMMON_RUN_TIME_H

#include <string>
#include "RPC/net/rpc/rpc_interface.h"

namespace RPC
{

    class RpcInterface;

    class RunTime
    {
    public:
        RpcInterface *getRpcInterface();

    public:
        static RunTime *GetRunTime();

    public:
        std::string m_msg_id;
        std::string m_method_name;
        RpcInterface *m_rpc_interface{NULL};
    };

}

#endif