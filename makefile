# 指定编译器  
CC = g++  
   
# 指定编译选项  
CFLAGS = -Wall -g -std=c++11
  
# 指定源文件和头文件所在的文件夹  
SRC_DIR = ./common/ ./testcases/  
INC_DIR = ./common/ ./testcases/  
   
# 指定链接时的库文件（如果有的话）  
LIBS =   
  
# 指定编译规则  
 %.o: %.cc
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@  
       
# 主程序  
./bin/log: $(patsubst %.cc,%.o,$(wildcard testcases/*.cc)) $(patsubst %.h,%.o,$(wildcard common/*.h common/*.cc))
	$(CC) $^ $(LIBS) -I$(INC_DIR) -o $@  
  
# 清理目标  
clean:
	rm -f *.o bin
