#include<ctime>
#include<sys/time.h>
#include<iostream>
#include<string>
#include<sstream>
#include<stdio.h>
#include<signal.h>
#include<assert.h>

#include "RPC/common/log.h"
#include "RPC/common/util.h"
#include "RPC/common/config.h"
#include "RPC/common/run_time.h"
#include "RPC/net/eventloop.h"

namespace RPC{
    
    Logger::Logger(LogLevel level, int type) : m_set_level(level), m_type(type){
        if(m_type == 0){
            return ;
        }
       
        m_async_logger = std::make_shared<AsyncLogger>(
                Config::GetGlobalConfig()->m_log_file_name + "_rpc",
                Config::GetGlobalConfig()->m_log_file_path,
                Config::GetGlobalConfig()->m_log_file_max_size);
        
        m_async_app_logger = std::make_shared<AsyncLogger>(
                Config::GetGlobalConfig()->m_log_file_name + "_app",
                Config::GetGlobalConfig()->m_log_file_path,
                Config::GetGlobalConfig()->m_log_file_max_size);
       
    }

    void Logger::init(){
        if(m_type == 0){
            return ;
        }
        
        m_timer_event = std::make_shared<TimerEvent>(Config::GetGlobalConfig()->m_log_sync_interval, true, std::bind(&Logger::syncLoop, this));

        EventLoop::GetCurrentEventLoop()->addTimerEvent(m_timer_event);
    }

    void Logger::syncLoop(){
        // 同步 m_buffer 到 aysnc_buffer 的队列尾部
        ScopeMutex<Mutex> lock(m_mutex);
        std::vector<std::string> tmp;
        m_buffer.swap(tmp);
        lock.unlock();
        if(!tmp.empty()){
            m_async_logger->pushLogBuffer(tmp);
        }
        tmp.clear();

        // 同步 m_app_buffer 到 aysnc_app_buffer 的队列尾部
        ScopeMutex<Mutex> app_lock(m_app_mutex);
        std::vector<std::string> tmp_app;
        m_app_buffer.swap(tmp_app);
        app_lock.unlock();
        if(!tmp_app.empty()){
            m_async_app_logger->pushLogBuffer(tmp_app);
        }
        tmp_app.clear();

    }

    std::string LogLevelToString(LogLevel level){
        switch (level)
        {
        case Debug:
            return "DEBUG";
        case Info:
            return "INFO";
        case Error:
            return "ERROR";
        default:
            return "UNKNOW";
        }
    }
   
    LogLevel StringToLogLevel(const std::string& log_level){
    	if(log_level == "DEBUG"){
            return Debug;	
        }else if(log_level == "INFO"){
            return Info;
        }else if(log_level == "ERROR"){
            return Error;	
        }else if(log_level == "UNKNOW"){
            return Unknow;
        }
        return Unknow;
    }

 
    std::string LogEvent::toString(){

        //struct timeval适用于精确测量时间间隔，而struct tm适用于表示和操作日历时间的各个成分。

        struct timeval now_time; // 定义一个名为now_time的timeval结构体变量，用于存储当前时间

        gettimeofday(&now_time,nullptr); // 获取当前时间并存储在now_time变量中

        struct tm now_time_t; // 定义一个名为now_time_t的tm结构体变量，用于存储当前时间的解析结果

        localtime_r(&(now_time.tv_sec),&now_time_t); // 使用当前时间的秒数来解析出当前时间的年月日时分秒，并存储在now_time_t变量中

        char buf[128]; // 定义一个长度为128的字符数组，用于存储格式化后的时间字符串

        strftime(&buf[0],128,"%y-%m-%d %H:%M:%S",&now_time_t); // 使用now_time_t中的时间信息按照指定的格式"%y-%m-%d %H:%M:%S"将时间格式化成字符串，存储在buf数组中

        std::string time_str(buf); // 将buf数组中的字符串转换为std::string类型

        int32_t ms = now_time.tv_usec / 1000; // 获取当前时间的微秒数，并将其转换为毫秒数

        time_str = time_str + "."  + std::to_string(ms).substr(0,3); // 将毫秒数拼接到时间字符串的末尾

        m_pid = getPId();
        m_thread_id = getThreadId();

        
        std::stringstream ss;

        ss << "[" << LogLevelToString(m_level) << "]\t"
            << "[" << time_str << "]\t"
            << "[" << m_pid << ":" << m_thread_id << "]\t";

        // 获取当前线程处理的请求的 msgid
        std::string msgid = RunTime::GetRunTime()->m_msg_id;
        std::string method_name = RunTime::GetRunTime()->m_method_name;
        if (!msgid.empty()) {
            ss << "[" << msgid << "]\t";
        }

        if (!method_name.empty()) {
            ss << "[" << method_name << "]\t";
        }
        return ss.str();
    }

    static Logger* g_logger = nullptr;
    Logger* Logger::GetGlobalLogger(){
        return g_logger;
    }
    void Logger::InitGlobalLogger(int type /*=1*/){
    	LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);	     
        printf("Init Log Levels [%s]\n",LogLevelToString(global_log_level).c_str());
	    g_logger = new Logger(global_log_level, type);
        g_logger->init();
    }

    void Logger::pushLog(const std::string& msg){
        if(m_type == 0){
            printf("%s\n",msg.c_str());
        }else{
            ScopeMutex<Mutex> lock(m_mutex);
            m_buffer.emplace_back(msg);
            lock.unlock();
        }
        
    }
    void Logger::pushAppLog(const std::string& msg){
        ScopeMutex<Mutex> lock(m_app_mutex);
        m_app_buffer.emplace_back(msg);
        lock.unlock();
    }

    void Logger::log(){
        // ScopeMutex<Mutex> lock(m_mutex);
        // std::vector<std::string> temp;
        // m_buffer.swap(temp);
        // lock.unlock();
        // while(!temp.empty()){
        //         std::string msg = temp.front();
        //         temp.pop_back();
        //         printf("%s",msg.c_str());
        //     }
        // lock.unlock();
    }

    AsyncLogger::AsyncLogger(const std::string& file_name, const std::string& file_path, int max_file_size)
    : m_file_name(file_name), m_file_path(file_path), m_max_file_size(max_file_size){
        sem_init(&m_sempahore, 0, 0);
        assert(pthread_create(&m_thread, NULL, &AsyncLogger::Loop, this) == 0);
        //pthread_create(&m_thread, NULL, &AsyncLogger::Loop, this);
        sem_wait(&m_sempahore);
    }

    void* AsyncLogger::Loop(void* arg){
        // 将buffer里的全部数据打印到文件中
        // 然后线程睡眠，直到有新的数据
        // 重复上述过程
        AsyncLogger* logger = reinterpret_cast<AsyncLogger*>(arg);
        assert(pthread_cond_init(&logger->m_condition, NULL) == 0);
        //pthread_cond_init(&logger->m_condition, NULL);
        sem_post(&logger->m_sempahore);

        while(true){
            ScopeMutex<Mutex> lock(logger->m_mutex);
            while(logger->m_buffer.empty()){
                pthread_cond_wait(&(logger->m_condition),logger->m_mutex.getMutex());
            }
            std::vector<std::string> tmp;
            tmp.swap(logger->m_buffer.front());
            logger->m_buffer.pop();

            lock.unlock();

            timeval now;
            gettimeofday(&now, NULL);

            struct tm now_time;
            localtime_r(&(now.tv_sec), &now_time);

            const char* format = "%Y%m%d";
            char date[32];

            strftime(date, sizeof(date), format, &now_time);

            if(std::string(date) != logger->m_date){
                logger->m_log_no = 0;
                logger->m_reopen_flag = true;
                logger->m_date = std::string(date);
            }
            if(logger->m_file_handler == NULL){
                logger->m_reopen_flag = true;
            }
            
            std::stringstream ss;
            ss << logger->m_file_path << logger->m_file_name << "_"
               << std::string(date) << "_log." ;

            std::string log_file_name = ss.str() + std::to_string(logger->m_log_no);
            if(logger->m_reopen_flag){
                if(logger->m_file_handler){
                    fclose(logger->m_file_handler);
                }
                logger->m_file_handler = fopen(log_file_name.c_str(), "a");
                logger->m_reopen_flag = false;
            }
            
            if(ftell(logger->m_file_handler) > logger->m_max_file_size){
                fclose(logger->m_file_handler);
                log_file_name = ss.str() + std::to_string(++logger->m_log_no);
                logger->m_file_handler = fopen(log_file_name.c_str(), "a");
                logger->m_reopen_flag = false;
            }

             for(auto& item : tmp){
                if(!item.empty()){
                    fwrite(item.c_str(), 1, item.length(), logger->m_file_handler);
                }
             }
             
             logger->flush();

             if(logger->m_stop_flag){
                return NULL;
             }
        }
        return NULL;
    }

    void AsyncLogger::stop(){
        m_stop_flag = true;
    }

    void AsyncLogger::flush(){
        if(m_file_handler){
            fflush(m_file_handler);
        }
    }

    void AsyncLogger::pushLogBuffer(std::vector<std::string>& vec){
        ScopeMutex<Mutex> lock(m_mutex);
        m_buffer.push(vec);
        lock.unlock();
        // 此时需唤醒异步日志线程
        pthread_cond_signal(&m_condition);
    }


}
