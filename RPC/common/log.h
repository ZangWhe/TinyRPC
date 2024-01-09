#ifndef TinyRPC_COMMON_LOG_H
#define TinyRPC_COMMON_LOG_H

#include<string>
#include<queue>
#include<vector>
#include<memory>
#include<string>
#include<iostream>
#include<sstream>
#include<semaphore.h>

#include "RPC/common/config.h"
#include "RPC/common/mutex.h"
#include "RPC/net/timer_event.h"

#pragma GCC diagnostic ignored "-Wformat-security"


namespace RPC{
    

    template<typename... Args>
    std::string formatString(const char* str,Args&&... args) {

        size_t size = snprintf(nullptr, 0, str, args...);
        
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
    RPC::Logger::GetGlobalLogger()->pushLog( (RPC::LogEvent(RPC::LogLevel::Debug).toString())                                   \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \

    #define INFOLOG(str, ...)                                                                                                   \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Info)                                                              \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( (RPC::LogEvent(RPC::LogLevel::Info).toString())                                    \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \


    #define ERRORLOG(str, ...)                                                                                                  \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Error)                                                             \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( (RPC::LogEvent(RPC::LogLevel::Error).toString())                                   \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \

    #define UNKNOWLOG(str, ...)                                                                                                 \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Unknow)                                                            \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushLog( ((new RPC::LogEvent(RPC::LogLevel::Unknow))->toString())                           \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \



    #define APPDEBUGLOG(str, ...)                                                                                                  \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Debug)                                                             \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushAppLog( (RPC::LogEvent(RPC::LogLevel::Debug).toString())                                   \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \

    #define APPINFOLOG(str, ...)                                                                                                   \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Info)                                                              \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushAppLog( (RPC::LogEvent(RPC::LogLevel::Info).toString())                                    \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \


    #define APPERRORLOG(str, ...)                                                                                                  \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Error)                                                             \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushAppLog( (RPC::LogEvent(RPC::LogLevel::Error).toString())                                   \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
    }                                                                                                                           \

    #define APPUNKNOWLOG(str, ...)                                                                                                 \
    if(RPC::Logger::GetGlobalLogger()->getLogLevel() <= RPC::Unknow)                                                            \
    {                                                                                                                           \
    RPC::Logger::GetGlobalLogger()->pushAppLog( ((new RPC::LogEvent(RPC::LogLevel::Unknow))->toString())                           \
    + "[" + std::string(__FILE__) + " : " + std::to_string(__LINE__) + "]\t" + RPC::formatString(str,##__VA_ARGS__) + '\n');    \
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

    class AsyncLogger{
        public:
            typedef std::shared_ptr<AsyncLogger> s_ptr;

            AsyncLogger(const std::string& file_name, const std::string& file_path, int max_file_size);

            void stop();

            void flush();   // 刷新到磁盘

            void pushLogBuffer(std::vector<std::string>& vec);

        public:
            static void* Loop(void*);

        private:
            std::queue<std::vector<std::string>> m_buffer;
            // m_file_name/m_file_name_yyyymmdd.0
            std::string m_file_name;
            std::string m_file_path;

            int m_max_file_size {0};    // 单个文件最大大小, 单位为字节

            sem_t m_sempahore;

            pthread_t m_thread;

            pthread_cond_t m_condition;

            Mutex m_mutex;

            std::string m_date; // 当前打印日志的文件日期

            FILE* m_file_handler {NULL};   // 当前打开的日志文件句柄

            bool m_reopen_flag {false};     // 是否需要重新打开的标志位

            int m_log_no {0};   // 日志文件序号

            bool m_stop_flag {false};
    };

    class Logger{
        public:
            typedef std::shared_ptr<Logger> s_ptr;

            Logger(LogLevel level);

            void init();

            void pushLog(const std::string& msg);

            void pushAppLog(const std::string& msg);

            void log();
	    
            LogLevel getLogLevel() const {
                return m_set_level;
            }

            void syncLoop();


        public:
            static Logger* GetGlobalLogger();
	        
            static void InitGlobalLogger();

        private:
            std::vector<std::string> m_buffer;

            std::vector<std::string> m_app_buffer;

            LogLevel m_set_level;
		    
            Mutex m_mutex;

            Mutex m_app_mutex;

            // m_file_name/m_file_name_yyyymmdd.1
            std::string m_file_name;
            std::string m_file_path;

            int m_max_file_size {0};    // 单个文件最大大小

            AsyncLogger::s_ptr m_async_logger;

            AsyncLogger::s_ptr m_async_app_logger;

            TimerEvent::s_ptr m_timer_event;
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
