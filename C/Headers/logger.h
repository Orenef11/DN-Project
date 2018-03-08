#ifndef RAFT_PROJECT_LOGGER_H
#define RAFT_PROJECT_LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG(var) #var,var

#define MAX_LOGGER_MSG 500

#define NULL_STRING "NULL VALUE"

#define INT_VALUES 0
#define CHARS_VALUES 1
#define NO_VALUES 2

#define __WRITE_TO_LOGGER__(write_to_logger,logger_level,problem_desc,msg_type,args_num,...) ( \
				add_logger_msg(write_to_logger,logger_level,__FILE__,__func__,__LINE__,problem_desc,msg_type,args_num,##__VA_ARGS__) )
				
int add_logger_msg(int (*write_to_logger)(int logger_level,char* logger_info),int logger_level,const char * file,const char *func_name,const int line,
				const char* problem_desc,int msg_type, int args_num,...);

#endif
