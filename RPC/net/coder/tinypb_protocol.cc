
#include "RPC/net/coder/tinypb_protocol.h"

namespace RPC{
    
    char TinyPBProtocol::PB_START = 0x02;
    char TinyPBProtocol::PB_END = 0x03;

    TinyPBProtocol::TinyPBProtocol(){

    }

    TinyPBProtocol::~TinyPBProtocol(){
        
    }

}