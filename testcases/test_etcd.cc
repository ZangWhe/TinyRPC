// #include <etcd/Watcher.hpp>
// #include <etcd/Client.hpp>
// #include<etcd/Response.hpp>
// #include <etcd/KeepAlive.hpp>
// #include <string>
// #include <iostream>


// #include <chrono>
// #include <thread>


// int main()
// {
//     etcd::Client etcd("http://127.0.0.1:12312");
//     pplx::task<etcd::Response> response_task1 = etcd.put("foo", "bar");
//     std::this_thread::sleep_for(std::chrono::seconds(3));
//     pplx::task<etcd::Response> response_task = etcd.get("foo");
//     // ... do something else
    
//     try
//   {
//     etcd::Response response = response_task.get(); // can throw
//     if (response.is_ok())
//       std::cout << "successful read, value=" << response.value().as_string();
//     else
//       std::cout << "operation failed, details: " << response.error_message();
//   }
//   catch (std::exception const & ex)
//   {
//     std::cout << "communication problem, details: " << ex.what();
//   }

//     return 0;
// }



#include <etcd/Client.hpp>
#include <etcd/Response.hpp>
#include <string>
#include <iostream>
int main() {
    try {
        // 创建etcd客户端
        etcd::Client etcd("127.0.0.1:2379");
        etcd.put("1234","9876");

        // 获取键值对
        pplx::task<etcd::Response> response_task = etcd.get("1234");

        etcd::Response response = response_task.get();
        std::cout << response.value().as_string()<<std::endl;
        
    
    } catch (const std::exception& ex) {
        std::cout << "Failed to connect to etcd: " << ex.what() << std::endl;
    }

    return 0;
}