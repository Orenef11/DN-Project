#ifndef RAFT_PROJECT_MAIN_H
#define RAFT_PROJECT_MAIN_H

#include "C/Headers/state_machine.h"
//#include "static_queue.h"
//#include "raft_server.h"
int run_raft(char* raft_ip,int raft_port,int server_id,int members_num,int leader_timeout,void(*set_callback_function)(void));

void transfer_callback_function(int (*add_to_log_DB)(int log_id,char* cmd,char* key,char* value),
								int (*update_DB)(char * DB_flag,char * key,char* value),
								char* (*get_log_by_diff)(int from,int to),
								int (*write_to_logger)(int logger_level,char * logger_info),
								int (*execute_log)(int last_log_index),
								int (*clear_log_from_log_id)(int log_id));
#endif //RAFT_PROJECT_MAIN_H
