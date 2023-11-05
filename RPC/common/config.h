#ifndef RPC_COMMON_CONFIG_H
#define RPC_COMMON_CONFIG_H



namespace RPC{

class Config{

public:
	Config(const char* xmlfile);
public:
	static Config* GetGlobalConfig();
	static void SetGlobalConfig(const char* xmlfile);
public:
	std::string m_log_level;	

};

}

#endif
