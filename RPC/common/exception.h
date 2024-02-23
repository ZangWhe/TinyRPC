#ifndef RPC_COMMON_EXCEPTION_H
#define RPC_COMMON_EXCEPTION_H

#include <exception>
#include <string>

namespace RPC {

class RPCException : public std::exception {
 public:

  RPCException(int error_code, const std::string& error_info) : m_error_code(error_code), m_error_info(error_info) {}

  // 异常处理
  // 当捕获到 RPCException 及其子类对象的异常时，会执行该函数
  virtual void handle() = 0;

  virtual ~RPCException() {};

  int errorCode() {
    return m_error_code;
  }

  std::string errorInfo() {
    return m_error_info;
  }

 protected:
  int m_error_code {0};

  std::string m_error_info;

};


}



#endif