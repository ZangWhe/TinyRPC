#include<stdio.h>
#include "RPC/common/log.h"
#include "RPC/common/config.h"

#include<pthread.h>
void* func(void*){
	DEBUGLOG("this is thread in %s","func_dubug");
	INFOLOG("this is thread in % s","func_info");
	return NULL;
}
int main(){
	RPC::Config::SetGlobalConfig("../conf/Tinyxml.xml");
	RPC::Logger::InitGlobalLogger();
	pthread_t thread;
	pthread_create(&thread,NULL,&func,NULL);
	DEBUGLOG("test debug log % s","11");
	INFOLOG("test info log % s","22");
    	
	pthread_join(thread,NULL);
	return 0;
}
