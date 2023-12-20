#ifndef TinyRPC_COMMON_LOG_H
#define TinyRPC_COMMON_LOG_H

#include<string>
#include<queue>
#include<memory>
#include<string>
#include<iostream>

#include "config.h"
#include "mutex.h"




namespace RPC{
    
    template<typename... Args>
    std::string formatString(const char* str,Args&&... args) {

        
        
        int size = snprintf(nullptr, 0, str, args...);
        
        std::string result;
        if (size > 0) {
            result.resize(size);
            
            snprintf(&result[0], size + 1, str, args...);
        }

        return result;
    }

    #define DEBUGLOG(str, ...)                                                                                                  \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Debug)                                                             \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( ((new RPC::LogEvent(RPC::LogLevel::Debug))->toString())                            \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    RPC::Logger::GetGlobalLogger()->log();                                                                                      \
    }                                                                                                                           \

    #define INFOLOG(str, ...)                                                                                                   \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Info)                                                              \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( ((new RPC::LogEvent(RPC::LogLevel::Info))->toString())                             \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    RPC::Logger::GetGlobalLogger()->log();                                                                                      \
    }                                                                                                                           \


    #define ERRORLOG(str, ...)                                                                                                  \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Error)                                                             \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( ((new RPC::LogEvent(RPC::LogLevel::Error))->toString())                            \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    RPC::Logger::GetGlobalLogger()->log();                                                                                      \
    }                                                                                                                           \

    #define UNKNOWLOG(str, ...)                                                                                                 \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Unknow)                                                            \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( ((new RPC::LogEvent(RPC::LogLevel::Unknow))->toString())                           \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    RPC::Logger::GetGlobalLogger()->log();                                                                                      \
    }                                                                                                                           \


    enum LogLevel{
        Unknow = 0,
	    Debug = 1,
        Info = 2,
        Error = 3
    };

    
    // 日志级别转字符串
    std::string LogLevelToString(LogLevel level);
    // 字符串转日志级别
    LogLevel StringToLogLevel(const std::string& log_level);

    

    class Logger{
        public:
            Logger(LogLevel level): m_set_level(level){}
	    typedef std::shared_ptr<Logger> s_ptr;
	    std::queue<std::string> m_buffer;

            void pushLog(const std::string& msg);
            void log();
	    
            LogLevel getLogLevel() const {
                return m_set_level;
            }


        public:
            static Logger* GetGlobalLogger();
	        static void InitGlobalLogger();
        private:
            LogLevel m_set_level;
		    Mutex m_mutex;	    
    };


    class LogEvent{
        public:

            LogEvent(LogLevel level):m_level(level){};
            //获取文件名
            std::string getFileName(){
                return m_file_name;
            }
            //获取日志级别
            LogLevel getLogLevel(){
                return m_level;
            }
            //输出日志
            std::string toString();

            //日志级别转字符串
            //std::string LogLevelToString(LogLevel level);
	        //字符串转日志级别
	        //LogLevel StringToLogLevel(const std::string& log_level);
        private:
            std::string m_file_name;        //文件名
            int32_t m_file_line;            //行号
            int32_t m_pid;                  //进程号
            int32_t m_thread_id;            //线程号
            LogLevel m_level;               //日志级别

            
        
    };

    
}

#endif
